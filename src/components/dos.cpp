/***************************************************************************
                          dos.cpp  -  description
Density of states multiplier
this component creates a density of states profile that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by n values
which are linearly interpolated
22-5-2008 changed linear interpolation to subsampling, this is now very close to a wiener filter since we clip those
frequencies to zero which are below some noise treshold
main difference with wiener is that we don't fit the region before the edge and after estop
which is a form of prior knowledge.
The subsampled dos function is mirrored to avoid edge artefacts at the edge where 1 would jump to 0 which is unphysical.

                             -------------------
    begin                : Sat Oct 26 2002
    copyright            : (C) 2002 by Jo Verbeeck
    email                : jo@localhost.localdomain
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//#define SUMRULE
//#define LINEAR

//#define COMPONENT_DEBUG
#include "src/components/dos.h"
#include "src/gui/integerinput.h"
#include "src/gui/componentselector.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "src/gui/graph.h"
#include "src/core/eelsmodel.h"
#include "src/gui/saysomething.h"
#include "src/core/fftw++.h"
#ifdef DEBUG
    #include "src/core/debug_new.h" //memory leak checker
#endif
using namespace fftwpp;


class QWorkspace;
//get two global pointers from main
extern QWorkspace* getworkspaceptr();
extern Eelsmodel* geteelsmodelptr();//main.cpp contains this global function with a pointer to eelsmodel
Dos::Dos() //create a dummy version
:Component()
{
  compptr=0;
  this->setname("Fine Structure (DOS)");
  this->setdescription("Fine Structure used in combination with a normal cross-section");
  degree=0;
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);
}

Dos::Dos(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
    realSUB=0;
    fourierSUB=0;
    real=0;
    fourier=0;
  compptr=0;
  if (parameterlistptr==0){
    //ask for the degree of the polynomial between 1 and 100
    const int min=1;
    const int max=1024;
    int d=10;
    Integerinput myinput(0,"","enter number of datapoints",d,min,max);
    degree=size_t(d);
    //enter Estart and Estop
    Parameter* p1=new Parameter("Estart",d,1);
    p1->interactivevalue("Enter Estart");
    p1->setchangeable(false);
    this->addparameter(p1);

    Parameter* p2=new Parameter("Estop",d,1);
    p2->interactivevalue("Enter Estop");
    p2->setchangeable(false);
    this->addparameter(p2);

    //Parameter* p=0;
    //create the parameters and store them in a vector and init them to 1.0
    for (size_t i=0;i<degree;i++){
      std::string name;
      std::ostringstream s;
      if ((s << "a"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     name=s.str();
      }
	Parameter* p=new Parameter(name,1.0,1);

      /*}
      try{
	p=new Parameter(name,1.0,1);
      }
      catch(...){
	//unable to allocate
	throw Componenterr::unable_to_create();
      }
      p->setboundaries(-10.0,10.0);
      this->addparameter(*p);
      */
      p->setboundaries(-1.0,20.0);
      p->setlinear(true); //these parameters are linear
      this->addparameter(p);
    }
    //now link this to a cross section which needs to be multiplied
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    int cnr=0;
   //create a componentselector here
    Componentselector myinput2(0,"","Select the component you want to multiply with",cnr);


    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
    set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
    if (compptr!=0){
      compptr->setmultiplierptr(this);
    }
    else{
      //something went wrong
      Saysomething mysay(0,"Error","the component didn't appear to be valid");
      throw Componenterr::unable_to_create();
    }
    //save this number in a parameter
    Parameter* p3=new Parameter("compnr",cnr,1);
    p3->setchangeable(false);
    this->addparameter(p3);
  }
  else{
    //get parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
      Parameter* p=(*parameterlistptr)[i];
      this->addparameter(p);
    }
    degree=(parameterlistptr->size())-3;//there are three other parameters
    //tell the component that we multiply that we are here
    Parameter* p3=(*parameterlistptr)[parameterlistptr->size()-1];
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    const int cnr=int(p3->getvalue());
    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
    set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
    if (compptr!=0) {
      compptr->setmultiplierptr(this);
    }
  }


  //give a name and description
  setname("Fine Structure (DOS)");
  setdescription("Fine Structure function used in combination with a normal cross-section");
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);


//create info for the subspace buffer that contains a mirrored version of the subsample dos
    preparesubspace();

  //prepare buffers for Fourier transforms of the upsampled DOS
  //twice as large as the spectrum to contain the mirror
  //create in and out buffer, out=complex
  real = FFTWdouble(2*this->getnpoints());
  fourier =FFTWComplex(this->getnpoints()+1);
 fft_plan=new rcfft1d(2*this->getnpoints(),real,fourier);
 ifft_plan=new crfft1d(2*this->getnpoints(),fourier,real);

   #ifdef SUMRULE
  //create an extra plot for sum rule work
  cumsumrule=new Spectrum(n,estart,dispersion);
  cumsumrule->setname("cumulative Bethe sum rule difference");
  cumsumrule->display(getworkspaceptr());
  #endif

  //show the current DOS
  calculate();
  setvisible(true);
  //tell that we have gradients for the dos points
  //parameters 2....degree+1
  for (size_t i=2;i<(degree+2);i++){
      #ifdef LINEAR
    this->sethasgradient(i,true);
    #endif
  }

  //show an equaliser to help graphically tune the parameters
  this->showequalizer();


}

Dos::~Dos(){
    //delete the FFTW buffers
  FFTWdelete(realSUB);
  FFTWdelete(fourierSUB);
  FFTWdelete(real);
  FFTWdelete(fourier);

  //killing the spectrum automatically cleans up the graph
  //tell the compptr that we are no longer multiplying it
  //check if the compptr is still valid, maybe it was killed and that is the reason why
  //we are getting deleted too
  try{
      const bool db=compptr->get_ismultiplier();
      (void) db;
      }
  catch(...){
      compptr=0;
  }

  if (compptr!=0){
    compptr->releasemultiplier();
  }

  #ifdef SUMRULE
  if (cumsumrule!=0) delete(cumsumrule);
  #endif
}

bool Dos::checkxsection(){
  //check if the cross section component we are pointing to has still the same number
  //otherwise update the component number parameter
  Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
  const int cnr=mymodel->getcomponentindexbypointer(compptr);
  if (compptr==NULL){
    return false;
  }
  Parameter* cnrptr= getparameter(degree+2);
  if ((cnr==-1)&&(compptr!=NULL)){
    //component not found also kill ourselves...
    Saysomething mysay(0,"Error","DOS: the component we are pointing to seems to be not present");
    compptr=NULL;
    cnrptr->setchangeable(true); //unlock
    cnrptr->setvalue(-1); //update non-existing value
    cnrptr->setchangeable(false); //lock
    return false;
  }
  cnrptr->setchangeable(true); //unlock
  cnrptr->setvalue(cnr); //update to current value
  cnrptr->setchangeable(false); //lock
  return true;
}


void Dos::calculate()
{
  if (!checkxsection()) return; //if a problem with the cross section we are pointing to, stop calculating

  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  //const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= getparameter(1);
  //const double Estop=Estopptr->getvalue();

  //calculate the DOS
  Parameter* pointptr=0; //pointer to a datapoint in the list
  bool changes=false;
  for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+2);
    changes=(changes||pointptr->changed()); //see if any of the data point parameters has changed
   }

  if (Estartptr->changed()||changes||Estopptr->changed()){

      #ifdef SUMRULE
      rescaleavg();
      #endif

    //if anything changed, we need to calculate, if not we leave without calculating
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating DOS \n degree: " << degree <<"\n";
    std::cout << "Estart: " << Estart <<"\n";
    std::cout << "Estop: " << Estop <<"\n";
    for (size_t index=0;index<degree;index++){
      std::cout<< " a" <<index<<" :"<<getparameter(index+2)->getvalue()<< "\n";
    }
    #endif



    #ifdef LINEAR
    double cts=0.0;
    for (size_t i=0;i<(this->getnpoints());i++){
      const double en=this->getenergy(i);
      //do a linear interpolation to the data points
      if (en<=Estart){
	cts=0.0;
      }
      if (en>Estop){
	cts=1.0;
      }
      if ((en>Estart)&&(en<=Estop)){
	//obtain cts via interpolation
	//obtain a tabulated point below and at jindex
	size_t jindex=0;
	double epos=0.0;
	const double edistance=(Estop-Estart)/double(degree+1);
	if (edistance<1e-3){
	  Saysomething mysay(0,"Error","Estart must be < Estop, not calculating");
	  return;
	}
	for(size_t j=0;j<degree;j++){
	  const double ej=Estart+double(j+1)*edistance;
	  if (ej<en){
	    jindex=j;
	    epos=en-ej;
	  }
	}
	if (jindex+1<degree){
	  const double Ajindex=getparameter(jindex+2)->getvalue();
	  const double Ajindexnext=getparameter(jindex+3)->getvalue();
	  cts=Ajindex+(Ajindexnext-Ajindex)*epos/edistance;
	}
	else{
	  if (jindex<degree){
	  const double Ajindex=getparameter(jindex+2)->getvalue();
	    cts=Ajindex+(1.0-Ajindex)*epos/edistance;
	  }
	  else{
	    cts=1.0;
	  }
	}
      }



    this->setcounts(i,cts);
    //setdatapoint(i,en,cts,0.0); don't write the energy, this causes trouble with getenergy in combination with eshift
    }
#endif
#ifndef LINEAR
    //try upsampling instead of linear interpolation
    if (Estartptr->changed()||Estopptr->changed()){
        preparesubspace();
    }
    //have to change nr of subspace to if Estart or Estop changes
    //make a function




    //create a subsampled space with N params between estart and estop and 0 before and 1 after
    for (size_t i=0;i<nsubspace;i++){
        if (i<pointsbefore){
            realSUB[i]=0.0;
        }
        else{
            if (i<(pointsbefore+degree)){
                const size_t index=i-pointsbefore;
                realSUB[i]=getparameter(index+2)->getvalue();
            }
            else{
                realSUB[i]=1.0;
            }
        }
    }
    //mirror it in second half of realsub
    for (size_t i=nsubspace;i<2*nsubspace;i++){
       realSUB[i]=realSUB[2*nsubspace-i-1];
    }


    //now Fourier transform this
    fft_planSUB->fft(realSUB,fourierSUB); //fft(subspace)

    //copy fourier part in upsampled space
    for (size_t i=0;i<(this->getnpoints()+1);i++){
        if (i<(nsubspace+1)){
            fourier[i]=fourierSUB[i]; //copy known frequencies to fourier space of upsampled signal
        }
        else
        {
            fourier[i]=0.0; //and pad rest with zeros
        }
    }

  //now transform back to real space
  ifft_plan->fft(fourier,real);

  //and copy the first half part of the spectrum to this spectrum, this is now the upsampled function
  //beware that FFTW does no scaling so we have to correct with 1/N
  const double scale=1.0/( double(2*nsubspace));
  for (size_t i=0;i<(this->getnpoints());i++){
    this->setcounts(i,real[i]*scale);
  }





#endif


    //correct for Bethe sum rule constraint
    #ifdef SUMRULE
    double withdos;
    double withoutdos;
    dosumrule(withdos, withoutdos);
    double ratio=1.0;
    if (withoutdos>1.0){
        ratio=withdos/withoutdos;
    }
    for (size_t i=0;i<(this->getnpoints());i++){
        const double en=this->getenergy(i);
      if ((en>Estart)&&(en<=Estop)){
          const double currval=this->getcounts(i);
          this->setcounts(i,currval/ratio);
      }
    }
    dosumrule(withdos, withoutdos);
    //then show differential sum rule
    showsumrule();
    #endif

    //set parameters as unchanged since last time we calculated
    this->setunchanged();

  }
  else{
    #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, i don't need to calculate again\n";
    #endif
  }
}

Spectrum* Dos::getgradient(size_t j){
  //get analytical partial derivative to parameter j and return pointer to spectrum containing the gradient

  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= getparameter(1);
  const double Estop=Estopptr->getvalue();

  int jindex=(int)j-2;
  if ((jindex<0)||(jindex>=(int)degree)){
    //don`t have a derivative for these
    throw Componenterr::bad_index();
    return &gradient;
  }

  const double edistance=(Estop-Estart)/double(degree+1);
  if (edistance<1e-3){
      Saysomething mysay(0,"Error","Estart must be < Estop, not calculating gradient");
      return &gradient;
  }

  //get energy of the parameter and energy of previous and next parameter
  const double  ej=Estart+double(jindex+1)*edistance;
  //get previous
  const double eprevious=Estart+double(jindex)*edistance;
  //get next
  const double enext=Estart+double(jindex+2)*edistance;

  //analytical derivative wrt parameter j
  for (unsigned int i=0;i<this->getnpoints();i++)
  {
      double en=this->getenergy(i);
      //if en>previous and < next we have a simple deriv
      //otherwise zero
      try{
	if ((en>eprevious)&&(en<enext)&&(en<Estop)&&(en>Estart)){
	  if (en<=ej){
	    gradient.setcounts(i,((en-eprevious)/edistance)*compptr->getcounts(i));
	  }
	  else{
	    gradient.setcounts(i,(1.0-(en-ej)/edistance)*compptr->getcounts(i) );
	  }
	}
	else{
	  gradient.setcounts(i,0.0);
	}
      }
      catch(...){
	throw Componenterr::bad_index();
      }
  }

  return &gradient;
}

Dos* Dos::clone()const{
  return new Dos(*this);
}
Dos* Dos::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new Dos(n,estart,dispersion,parameterlistptr);
}
 void Dos::dosumrule(double &withdos, double &withoutdos){
     const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Estopptr= getparameter(1);
    const double Estop=Estopptr->getvalue();

     //calculate Bethe sume rule for cross section with or without dos
     withdos=0.0;
     withoutdos=0.0;
     //const double betasq=compptr->get //proble m getting collection angle
     //for now just leave it uncorrected, this should be OK for
     //core edges since thetaE doesn't change so much in the few 10eV above edge onset
  for (size_t i=0;i<this->getnpoints();i++)
  {
      const double en=this->getenergy(i);
      if ((en>Estart)&&(en<=Estop)){

        const double xsection=compptr->getcounts(i);

        //const double thetaesq=
        //const double correction=log(1+betasq/thetaesq); //collection angle correction
        const double correction=1.0;
        const double dos=this->getcounts(i);
        withdos+=en*dos*xsection*correction;
        withoutdos+=en*xsection*correction;
      }
  }
#ifdef COMPONENT_DEBUG
   std::cout << "Bethe sum rule with dos="<<withdos<<"\n";
   std::cout << "Bethe sum rule without dos="<<withoutdos<<"\n";
   std::cout << "Ratio with/without="<<withdos/withoutdos<<"\n";
#endif



 }
void Dos::showsumrule(){
    //copy the difference of the cumulative sum between with and without dos in a graph
     double withdos=0.0;
     double withoutdos=0.0;
    for (size_t i=0;i<this->getnpoints();i++){
      const double en=this->getenergy(i);
      const double xsection=compptr->getcounts(i);
      const double correction=1.0;
      const double dos=this->getcounts(i);
      withdos+=en*dos*xsection*correction;
      withoutdos+=en*xsection*correction;
      cumsumrule->setcounts(i,withdos-withoutdos);
    }
    cumsumrule->display();
}
void Dos::rescaleavg(){
    //scale parameters to avg, this is handy if you do the calibration to the sum rule
    //since otherwise the scaling has no true meaning and the parameters start to drift off towards the boundaries
Parameter* pointptr=0;
    //determine avg
    double avg=0.0;
    for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+2);
    avg+=pointptr->getvalue();
   }
   avg=avg/degree;
   //apply it to the points
   for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+2);
    const double newval=pointptr->getvalue()/avg;
    pointptr->setvalue(newval);
   }

}
void Dos::preparesubspace(){
    //create a subsampled space, do this whenever Estart or Estop changes
    //we could easily cope with changing number of parameters
    //but the only question is how to do this in the user interface???

  //total number of points in this space
    const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Estopptr= getparameter(1);
    const double Estop=Estopptr->getvalue();
    subdispersion=(fabs(Estart-Estop))/double(degree);
    nsubspace=size_t(fabs(this->getenergy(0)-this->getenergy(this->getnpoints()-1))/subdispersion);
    pointsafter=size_t((this->getenergy(this->getnpoints()-1)-Estop)/subdispersion);
    pointsbefore=size_t((Estart-this->getenergy(0))/subdispersion);
    if (pointsafter>nsubspace){
        pointsafter=nsubspace;
    }
    


    #ifdef COMPONENT_DEBUG
    std::cout << "DOS creating subspace subdispersion: " << subdispersion <<"\n";
    std::cout << "pointsbefore: " << pointsbefore <<"\n";
    std::cout << "pointsafter: " << pointsafter <<"\n";
    std::cout << "nsubspace: " << nsubspace <<"\n";
    #endif

    //and prepare the fourier stuff

    if (realSUB!=0){
        FFTWdelete(realSUB);
        FFTWdelete(fourierSUB);
    }

    realSUB = FFTWdouble(2*nsubspace);
    fourierSUB =FFTWComplex(nsubspace+1);

    //make a plan for Fourier transforms
    fft_planSUB=new rcfft1d(2*nsubspace,realSUB,fourierSUB);
    ifft_planSUB=new crfft1d(2*nsubspace,fourierSUB,realSUB);

}

