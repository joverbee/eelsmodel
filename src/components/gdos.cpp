/***************************************************************************
                          gdos.cpp  -  description
Generic Density of states multiplier
this component creates a density of states profile that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by n values

several options can be chosen for the function
-linear interpolation
-cubic interpolation
-upsampling
-non-equidistant energy points
-enforcing Bethe sum rule

this class replaces all older DOSses and combines them in 1 component

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

//#define LINEAR
#define MASS_CENTER
//#define COMPONENT_DEBUG

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_vector.h>

#include <QDialog>

#include "src/components/gdos.h"

#include "src/core/fftw++.h"
#include "src/core/eelsmodel.h"
#include "src/core/monitor.h"
#include "src/core/parameter.h"

#include "src/gui/integerinput.h"
#include "src/gui/componentselector.h"
#include "src/gui/graph.h"
#include "src/gui/saysomething.h"
#include "src/gui/GDOSoptions.h"

using namespace fftwpp;

class QWorkspace;

QWorkspace* getworkspaceptr();
Eelsmodel* geteelsmodelptr();

GDos::GDos() //create a dummy version
:Component(),Evector(),Yvector(),b(),c(),d()
{
  mptr=0;
  mptr2=0;
  compptr=0;
  cumsumrule=0;
  this->setname("Generic Fine Structure (DOS)");
  this->setdescription("Fine Structure used in combination with a normal cross-section");
  degree=0;
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);
  acc=0;
  sp=0;
  Plotspec=0;
  smoothvalue=5.0;
  threshold=0.25;
}

GDos::GDos(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),Evector(),Yvector(),b(),c(),d()
{
    mptr=0;
    mptr2=0;
    acc=0;
    sp=0;
    Plotspec=0;
    realSUB=0;
    fourierSUB=0;
    real=0;
    fourier=0;
    compptr=0;
    dosumrule=false;
    cumsumrule=0;
    broadeningtype=QUADRATIC;
    interpolationtype=1;
    offset=2;
    smoothvalue=5.0;
    threshold=0.25;
    //create spectrum with same energy scale as this
    dummy=new Spectrum(n,estart,dispersion);



  if (parameterlistptr==0){
    //ask for the degree of the polynomial between 1 and 100
   // const int min=1;
    //const int max=1024;
    //show the options to the user
    degree=10;

    std::string optionstring;
    optionstring=interactiveoptions();


    //enter Estart and Estop

    Parameter* p1=new Parameter("Estart",this->getenergy(0),1);
    p1->interactivevalue("Enter Estart");
    p1->setchangeable(false);
    this->addparameter(p1);

    Parameter* p2=new Parameter("Estop",this->getenergy(this->getnpoints()-1),1);
    p2->interactivevalue("Enter Estop");
    p2->setchangeable(false);
    this->addparameter(p2);

    //store the options in the 3rd param
    Parameter* p3=new Parameter(optionstring,getoptions(),0);
    this->addparameter(p3);

    Parameter* p4=new Parameter("Alpha",0.0,1);
    p4->interactivevalue("Enter alpha");
    p4->setchangeable(false);
    this->addparameter(p4);


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
    Parameter* p5=new Parameter("compnr",cnr,1);
    p5->setchangeable(false);
    this->addparameter(p5);

   
    
  }
  else{
    //get parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
      Parameter* p=(*parameterlistptr)[i];
      this->addparameter(p);
    }
    degree=(parameterlistptr->size())-5;//there are 5 other parameters
    //tell the component that we multiply that we are here
    Parameter* p5=(*parameterlistptr)[parameterlistptr->size()-1];
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    const int cnr=int(p5->getvalue());
    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
    set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
    if (compptr!=0) {
      compptr->setmultiplierptr(this);
    }
    //get options from the options code
    makeoptions();
  }

  //add monitor to show the mass center
     Parameter* aptr=getparameter(3);
     Parameter* bptr=getparameter(2);
     mptr=new Monitor(aptr,aptr,3); //make a monitor on a param to show result of mass center
     mptr2=new Monitor(bptr,bptr,3); //make a monitor on a param to show result of area under edge
 


  //give a name and description
  setname("Generic Fine Structure (DOS)");
  setdescription("Fine Structure function used in combination with a normal cross-section");
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);

    //create info for the subspace buffer that contains a mirrored version of the subsample gdos
    //prepare buffers for Fourier transforms of the upsampled DOS
    //twice as large as the spectrum to contain the mirror
    //create in and out buffer, out=complex
    real = FFTWdouble(2*this->getnpoints());
    fourier =FFTWComplex(this->getnpoints()+1);
    fft_plan=new rcfft1d(2*this->getnpoints(),real,fourier);
    ifft_plan=new crfft1d(2*this->getnpoints(),fourier,real);

    setvisible(true);//do this before initDOS
    initDOS();

    //show the current DOS
    calculate();




  //show an equaliser to help graphically tune the parameters
  this->showequalizer();



}

GDos::~GDos(){
  if (mptr!=0) delete(mptr);
  if (mptr2!=0) delete(mptr2);
  //delete the FFTW buffers
  if (realSUB!=0) FFTWdelete(realSUB);
  if (fourierSUB!=0) FFTWdelete(fourierSUB);
  if (real!=0) FFTWdelete(real);
  if (fourier!=0) FFTWdelete(fourier);

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

  if (cumsumrule!=0) delete(cumsumrule);
  //clean up the gsl stuff
  if (sp!=0) gsl_spline_free (sp);
  if (acc!=0) gsl_interp_accel_free (acc);
  if (Plotspec!=0) delete(Plotspec);


}

bool GDos::checkxsection(){
  //check if the cross section component we are pointing to has still the same number
  //otherwise update the component number parameter
  Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
  const int cnr=mymodel->getcomponentindexbypointer(compptr);
  if (compptr==NULL){
    return false;
  }
  Parameter* cnrptr= getparameter(degree+4);
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


void GDos::calculate()
{
  if (!checkxsection()) return; //if a problem with the cross section we are pointing to, stop calculating

  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= getparameter(1);
  const double Estop=Estopptr->getvalue();

  //calculate the DOS
  Parameter* pointptr=0; //pointer to a datapoint in the list
  bool changes=false;
  for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+4);
    changes=(changes||pointptr->changed()); //see if any of the data point parameters has changed
   }

  if (Estartptr->changed()||changes||Estopptr->changed()){

      if (dosumrule){
      //rescaleavg();
      }

    //if anything changed, we need to calculate, if not we leave without calculating
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating DOS \n degree: " << degree <<"\n";
    std::cout << "Estart: " << Estart <<"\n";
    std::cout << "Estop: " << Estop <<"\n";
    for (size_t index=0;index<degree;index++){
      std::cout<< " a" <<index<<" :"<<getparameter(index+4)->getvalue()<< "\n";
    }
    #endif
    double cts=0.0;
    Parameter* aptr=this->getparameter(3);
    if ((Estartptr->changed()||Estopptr->changed())||aptr->changed()){
        //setup a new energy grid
        initDOS();
    }
    //copy the parameter values in the spline vectors
    copyparameters();
    const double scale=1.0/( double(2*nsubspace));
    switch(interpolationtype){
        case 1:


            //linear type
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
                    for(size_t j=0;j<degree;j++){
                        //const double ej=Estart+double(j+1)*edistance;
                        const double ej=Evector[j+offset]; //allows for lifetime broadening
                        if (ej<en){
                            jindex=j;
                            epos=en-ej;
                        }
                    }
                    if (epos<=0.0){
                        //between first and 0 point
                        const double edistance=Evector[offset]-Estart;
                        const double Ajindex=getparameter(4)->getvalue();
                        cts=(Ajindex)*fabs(epos)/edistance;
                    }
                    else{
                    if (jindex+1<degree){
                        const double edistance=Evector[jindex+1+offset]-Evector[jindex+offset];
                        const double Ajindex=getparameter(jindex+4)->getvalue();
                        const double Ajindexnext=getparameter(jindex+5)->getvalue();
                        cts=Ajindex+(Ajindexnext-Ajindex)*epos/edistance;
                    }
                    else{
                        if (jindex==(degree-1)){
                            //last interval to 1
                            const double edistance=Estop-Evector[jindex+offset];

                            const double Ajindex=getparameter(jindex+4)->getvalue();
                            cts=Ajindex+(1.0-Ajindex)*epos/edistance;
                        }
                        else{
                            cts=1.0;
                        }
                    }
                    }
                }
                this->setcounts(i,cts);
                //setdatapoint(i,en,cts,0.0); don't write the energy, this causes trouble with getenergy in combination with eshift
            }
            break;
        case 2:
            //cubic spline type
            dospline();
            //do the evaluation
            for (size_t i=0; i<this->getnpoints();i++){
                const double en=this->getenergy(i);
                if ((en>Estart)&(en<Estop)){
                    cts=seval(en);
                }
                if (en<=Estart){
                    cts=0.0;
                }
                if (en>=Estop){
                    cts=1.0;
                }
                this->setcounts(i,cts);
            }
            break;
        case 3:
            //upsample type
            //create a subsampled space with N params between estart and estop and 0 before and 1 after
            for (size_t i=0;i<nsubspace;i++){
                if (i<=pointsbefore){ //13-2-09 added = sign to fix shift
                    realSUB[i]=0.0;
                }
                else{
                    if (i<(pointsbefore+degree)){
                        const size_t index=i-pointsbefore;
                        realSUB[i]=getparameter(index+4)->getvalue();
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
                else{
                    fourier[i]=0.0; //and pad rest with zeros
                }
            }

            //now transform back to real space
            ifft_plan->fft(fourier,real);

            //and copy the first half part of the spectrum to this spectrum, this is now the upsampled function
            //beware that FFTW does no scaling so we have to correct with 1/N

            for (size_t i=0;i<(this->getnpoints());i++){
                this->setcounts(i,real[i]*scale);
            }
            break;
        default:
            //unknown, go to linear
            interpolationtype=1;
    }







 //correct for Bethe sum rule constraint
    if (dosumrule){
        double withdos;
        double withoutdos;
        /*gdosumrule(withdos, withoutdos);
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
*/
        gdosumrule(withdos, withoutdos);
        //then show differential sum rule
        showsumrule();
    }



    //set parameters as unchanged since last time we calculated
    this->setunchanged();

  }
  else{
    #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, i don't need to calculate again\n";
    #endif
  }
}

Spectrum* GDos::getgradient(size_t j){
  //get analytical partial derivative to parameter j and return pointer to spectrum containing the gradient
	
	
	//TODO check gradients, they seem to cause a memory allocation problem in the fitter
	


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

GDos* GDos::clone()const{
  return new GDos(*this);
}
GDos* GDos::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new GDos(n,estart,dispersion,parameterlistptr);
}
 void GDos::gdosumrule(double &withgdos, double &withoutgdos){
     const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Estopptr= getparameter(1);
    const double Estop=Estopptr->getvalue();

     //calculate Bethe sume rule for cross section with or without gdos
     withgdos=0.0;
     withoutgdos=0.0;
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
        const double gdos=this->getcounts(i);
        withgdos+=en*gdos*xsection*correction;
        withoutgdos+=en*xsection*correction;
      }
  }


#ifdef COMPONENT_DEBUG
   std::cout << "Bethe sum rule with gdos="<<withgdos<<"\n";
   std::cout << "Bethe sum rule without gdos="<<withoutgdos<<"\n";
   std::cout << "Ratio with/without="<<withgdos/withoutgdos<<"\n";
#endif



 }
void GDos::showsumrule(){
    //copy the difference of the cumulative sum between with and without gdos in a graph
     double withgdos=0.0;
     double withoutgdos=0.0;
     const Parameter* Estartptr= getparameter(0);
         const double Estart=Estartptr->getvalue();
         //const Parameter* Estopptr= getparameter(1);
         //const double Estop=Estopptr->getvalue();
    for (size_t i=0;i<this->getnpoints();i++){
      const double en=this->getenergy(i);
      const double xsection=compptr->getcounts(i);
      const double correction=1.0;
      const double gdos=this->getcounts(i);
      if (en>Estart){   
    	  withgdos+=en*gdos*xsection*correction;
    	  withoutgdos+=en*xsection*correction;
      }
      cumsumrule->setcounts(i,withgdos-withoutgdos);
    }
    cumsumrule->display();
}
void GDos::rescaleavg(){
    //scale parameters to avg, this is handy if you do the calibration to the sum rule
    //since otherwise the scaling has no true meaning and the parameters start to drift off towards the boundaries
Parameter* pointptr=0;
    //determine avg
    double avg=0.0;
    for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+4);
    avg+=pointptr->getvalue();
   }
   avg=avg/degree;
   //apply it to the points
   for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+4);
    const double newval=pointptr->getvalue()/avg;
    pointptr->setvalue(newval);
   }

}
void GDos::preparesubspace(){
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

void GDos::setoptions(){
    //call a function to set the options of the DOS
    std::string optionstring=interactiveoptions();
    storeoptions(optionstring);
    initDOS();
    //force an update of the model
    Parameter* firstptr= getparameter(4);
    firstptr->forcechanged(); //otherwise the model doesn't want to calculate
    geteelsmodelptr()->componentmaintenance_doupdate(); //redraw the parameters in componentmaintenance
    geteelsmodelptr()->componentmaintenance_updatescreen();
    geteelsmodelptr()->slot_componentmaintenance_updatemonitors();//make sure the monitors are recalculated
}

void GDos::initDOS(){
    //prepare all for the DOS to work with the given options

    //create an extra plot for sum rule work
    if (cumsumrule!=0){
        delete(cumsumrule);
        cumsumrule=0;
    }
    if (dosumrule){
        cumsumrule=new Spectrum(this->getnpoints(),this->getenergy(0),this->getdispersion());
        cumsumrule->setname("cumulative Bethe sum rule difference");
        cumsumrule->display(getworkspaceptr());
    }

    InitEnergy(); //prepare the energy points that are linked to the parameters
    //create a special plot
    if (Plotspec!=0){
        //remove it from the plot first
        this->getgraphptr()->removelastgraph();
        Plotspec->resize(Evector.size());        
    }
    else{
    	Plotspec=new Spectrum(Evector.size());
    }
   
    initplotspec();

    (this->getgraphptr())->addgraph(Plotspec);
    (this->getgraphptr())->setstyle(1,2); //set style of this plot to dots instead of lines

    switch(interpolationtype){
        case 1:
            break;
        case 2:
            //cubic spline type
            gslinit(); //setup the memory for the gsl fitter
            break;
        case 3:
            //upsample type
            preparesubspace();
            break;
        default:
            //unknown, go to linear
            interpolationtype=1;
        }

    //we only have analytical gradients in case of linear energy sampling
    //tell that we have gradients for the gdos points
    //parameters 4....degree+4
    for (size_t i=4;i<(degree+4);i++){
    	//this->sethasgradient(i,(broadeningtype==CONSTANT)&&(interpolationtype==1));
    	//gradients don't seem to work, they cause a memory alocation problem????
    	this->sethasgradient(i,false);
    }    
}

std::string GDos::interactiveoptions(){
     GDosoptions* myoptions=new GDosoptions(getworkspaceptr(),"",degree,interpolationtype,dosumrule,(size_t)broadeningtype,smoothvalue,threshold);
        const int result=myoptions->exec();
        std::string optionstring="";
        if (result==1){
            //OK pressed
            size_t olddegree=degree;
            degree=myoptions->getdegree();
            dosumrule=myoptions->getsumrule();
            interpolationtype=myoptions->getinterpolation();
            smoothvalue=myoptions->getintegrationwidth();
            threshold=myoptions->getthreshold();
            broadeningtype=(broadening)myoptions->getbroadeningtype();
            if (interpolationtype==3){
                broadeningtype=CONSTANT;
                optionstring="Constant broadening";
            }

            //do consistency check
            if ((degree<4)||(degree>this->getnpoints())){
                degree=4;
            }

            if (olddegree!=degree){
                //degree changed, change number of parameters and put all parameters to 1
                //but only if this component already exists
                if (this->get_nr_of_parameters()>4){
                    changedegree(olddegree,degree);
                }
            }
            if ((threshold<0.0)||(threshold>1.0)){
                threshold=0.5;
            }
        }
       //create an optionstring
        switch(broadeningtype){
                case LINEAR:
                    optionstring="Linear coefficient";
                    break;
                case QUADRATIC:
                    optionstring="Quadratic coefficient";
                    break;
                case EGERTON:
                    optionstring="Egerton Broadening atomic distance [nm]";
                    break;
                default:
                    optionstring="Constant broadening";
                    broadeningtype=CONSTANT;
                    break;
        }

        return optionstring;
     }
double GDos::getoptions(){
     //create a coded options code to store in a parameter
     double options=0;
     if (dosumrule){
         options=double(interpolationtype)+threshold;
    }else{
        options=-double(interpolationtype)-threshold;
    }
return options;
}
void GDos::storeoptions(std::string optionstring){
            Parameter* optionptr= getparameter(2);
            optionptr->setchangeable(true);
            optionptr->setvalue(getoptions());
            optionptr->setname(optionstring);
            optionptr->setchangeable(false);
}
void GDos::makeoptions(){
    Parameter* optionptr= getparameter(2);
    double options=optionptr->getvalue();
    threshold=fabs(options)-std::floor(fabs(options));   //treshold stored as behind the comma between 0 and 1
    if ((threshold<=0.0)||(threshold>1.0)){
        threshold=0.25;
    }
    if (options<0.0){
        dosumrule=false;
        interpolationtype=size_t(std::floor(fabs(options)));
    }
    else{
        dosumrule=true;
        interpolationtype=size_t(std::floor(fabs(options)));
    }
    //consistency check
    if ((interpolationtype==0)||(interpolationtype>3)){
        //unknown type, reset to piecewise linear
        interpolationtype=1;
    }
}

 void GDos::changedegree(size_t olddegree,size_t degree){
     //change number of degrees




     //take component pointer
     Parameter* cnrptr= getparameter(olddegree+4);
     double cnr=cnrptr->getvalue();

     //remove it from the list
     this->pullparameter();


     if (degree>olddegree){
         //add new points at end if olddegree<degree
         for (size_t i=0;i<(degree-olddegree);i++){
            std::string name;
            std::ostringstream s;
            if ((s << "a"<< i+olddegree)){ //converting an int to a string in c++ style rather than unsafe c-style
                name=s.str();
            }
            Parameter* p=new Parameter(name,1.0,1);
            p->setboundaries(-1.0,20.0);
            p->setlinear(true); //these parameters are linear
            this->addparameter(p);
        }
    }
    else{
        //else remove them
        for (size_t i=0;i<(olddegree-degree);i++){
            this->pullparameter();
        }
    }
     //and add componentpointer again at the end
    Parameter* p5=new Parameter("compnr",cnr,1);
    p5->setchangeable(false);
    this->addparameter(p5);
    //tell the model that something changed
    //this kills the stored values however
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    mymodel->resetstorage();
    this->showequalizer(); //redraw the equaliser with new number of sliders
}
double  GDos::Lifetimebroadening(double E){
  //calculate Lifetime broadening in eV according to Egerton 2007
  //E is the energy in eV above the onset

    const double dEmin=this->getdispersion(); //the minimum energy step is the dispersion
    const double dEmax=100.0; //maximum lifetime broadening, more doesn't make sense
    const double epsilon=E;
    const double h=4.13566733e-15; //eV/s
    const double m0=9.10938188e-31; //electron mass in kg
    const double e=1.60217646e-19; //electron charge (C)
    const double m=m0;
    const Parameter* aptr=getparameter(3);
    const Parameter* optptr=getparameter(2);
    const double a=aptr->getvalue(); //atomic diameter in nm
    double lambda=0.0;
    double v=0.1;

    double tau=0.0;
    const double pi=acos(-1.0);
    const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Estopptr= getparameter(1);
    const double Estop=Estopptr->getvalue();
    //#ifdef COMPONENT_DEBUG
    //  std::cout <<"Z="<<Z<<" rho="<<rho<<" \n";
    //#endif
    broadeningtype=CONSTANT;
    if (optptr->getname()=="Linear coefficient") broadeningtype=LINEAR;
    if (optptr->getname()=="Quadratic coefficient") broadeningtype=QUADRATIC;
    if (optptr->getname()=="Egerton Broadening atomic distance [nm]") broadeningtype=EGERTON;
    if (a==0.0) broadeningtype=CONSTANT;
    double dE;

switch(broadeningtype)
{
    case EGERTON:
        //Egerton broadening
        if (epsilon>this->getdispersion()){
            v=sqrt(2.0*e*epsilon/m); //speed
        }
        lambda=538.0*fabs(a)*pow(fabs(epsilon),-2.0)+0.41*pow(fabs(a),3.0/2.0)*sqrt(fabs(epsilon));
        tau=lambda*1e-9/v; //lifetime in seconds
        //#ifdef COMPONENT_DEBUG
        //std::cout <<"v="<<v<<" tau="<<tau<<" lambda"<<lambda<<" a="<<a<<" \n";
        //#endif

        dE=h/(2.0*pi*tau);
        //if dE lower than dispersion or E< Eonset
       break;
    case QUADRATIC:
        //Quadratic broadening
        dE=fabs(a)*pow(fabs(epsilon),2.0); // a simple quadratic broadening
        //check for infinity
        break;
    case LINEAR:
        //broadening linear with energy above onset
         dE=fabs(a)*fabs(epsilon); // a simple linear broadening
         break;
    case CONSTANT:
    default:
        dE=(Estart-Estop)/degree;
        break;
}

if (dE<dEmin){
    dE=dEmin;
}
if (E<=0){
    dE=dEmin;
}
if (dE>dEmax){
    dE=dEmax;
}

//#ifdef COMPONENT_DEBUG
//std::cout <<"Returning a lifetime broadening of dE="<<dE<<"for energy E="<<E<<"\n";
//#endif
return dE;
}
void GDos::InitEnergy(){
#ifdef COMPONENT_DEBUG
    std::cout <<"Initialising Energy points\n";
#endif
  //do this at the start and whenever the Estart or Estop changes
  size_t Eminid=0;
  size_t Emaxid=0;
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  Parameter* Estopptr= this->getparameter(1);
  double Estop=Estopptr->getvalue();

  for (size_t bin=0;bin<this->getnpoints();bin++){
    if (this->getenergy(bin)<=Estart){
      Eminid=bin;
    }
    if (this->getenergy(bin)<=Estop){
      Emaxid=bin;
    }
  }
#ifdef COMPONENT_DEBUG
  std::cout <<"Eminid: "<<Eminid<<"\n";
  std::cout <<"Emaxid: "<<Emaxid<<"\n";
#endif
    // copy parameters in Yvector and initialise
    Yvector.clear();
    Evector.clear();

    //add a first point that is zero and has energy Estart
    //if offset=2 add another point to make sure the aproach is flat
    if (offset==2){
        Yvector.push_back(0.0);
        Evector.push_back(Estart-1.0);
    }
    Yvector.push_back(0.0);
    Evector.push_back(Estart);
    //add points defined by the parameters
    //determine where we end up in terms of E when doing the Energy sum
    double Eposp=Estart;
    for (size_t index=0;index<=degree;index++){//do one more to end up on the point that should be Estop
        Eposp=Eposp+Lifetimebroadening(Eposp-Estart);
    }
    //Eposp is now the energy where we would end if taking the broadening
    //we want the energy to end at Estop
    //so we rescale with
    const double scale=(Eposp-Estart)/(Estop-Estart);
    double Epos=Estart;
    for (size_t index=0;index<degree;index++){
        //copy the parameter values in a vector
        Yvector.push_back((this->getparameter(index+4))->getvalue());
        Epos=Epos+Lifetimebroadening(Epos-Estart);
        if (interpolationtype!=3){
            Evector.push_back(Estart+(Epos-Estart)/scale);
        }
        else{
            Evector.push_back(Estart+((double)(index)+0.5)*(Estop-Estart)/(double)(degree));
        }

    }

    //recalculate Estop

    //add a last point that is 1 at Estart
    Yvector.push_back(1.0);
    Evector.push_back(Estop);
    //a second last point to make sure the end of the spline aproaches the tail with a flat section
    if (offset==2){
        Yvector.push_back(1.0);
        Evector.push_back(Estop+1.0);
    }

#ifdef COMPONENT_DEBUG
std::cout <<"Evector\n";
for (size_t i=0;i<Evector.size();i++){
  std::cout <<"Evector[ "<<i<<"]="<<Evector[i]<<"\n";
}
#endif

}

void GDos::copyparameters(){
    //copy the paramters values in the Yvector
    for (size_t index=0;index<degree;index++){
        //copy the parameter values in a vector, remember that point Yvector[0] is the first point but not connected to the parameters
        Yvector[index+offset]=((this->getparameter(index+4))->getvalue());
    }
}
void GDos::dospline(){
    //calculate the spline coefficients
     gsl_spline_init (sp, &Evector[0], &Yvector[0], Evector.size());
}

double GDos::seval(double x){
    return gsl_spline_eval (sp, x, acc);
}


void GDos::gslinit(){
    //init a cubic spline
    acc= gsl_interp_accel_alloc ();
    sp= gsl_spline_alloc (gsl_interp_cspline, Evector.size());
}
void GDos::initplotspec(){
    //copy Yvector and Evector in plotspec
    if (Plotspec!=0){
        for (size_t i=0;i<Plotspec->getnpoints();i++){
            Plotspec->setdatapoint(i,Evector[i],Yvector[i],0.0);
        }
        (this->getgraphptr())->updategraph(1,Plotspec);
    }
}

//inherit show but also redefine what should happen with the dos window
void GDos::show(){
    Component::show(); //do the normal show
    if (Plotspec!=0){
        initplotspec(); //but also update the points on the plot

        //copy energy values in the names of the parameters
        //this gives users the possiblity to know at what energy the dos points where taken
        for (size_t index=0;index<degree;index++){
            Parameter* pointptr=getparameter(index+4);
            std::string namestring;
            std::ostringstream s;
            if (s << "a" << index <<" @ " << Evector[index+offset] <<" eV"){ //converting an int to a string in c++ style rather than unsafe c-style
                // conversion worked
                namestring=s.str();
            }
            pointptr->setname(namestring);
        }
    }
}

void GDos::orderchanged(){
    //in case the order of the components changes, update the compnr
    const Model* mymodel=geteelsmodelptr()->getmodel();
    const int cnr=mymodel->getcomponentindexbypointer(compptr);
    Parameter* cnrptr= getparameter(degree+4);
    cnrptr->setchangeable(true); //unlock
    cnrptr->setvalue(cnr); //update to current value
    cnrptr->setchangeable(false); //lock
}

double GDos::masscenter(){
    const double integrationwidth=500.0;
    mptr=getparameter(3)->getmonitor();  
    if (mptr!=0){
        //calculate mass center of edge with fine structure
        //include extrapolated power law if needed
        const Parameter* Estartptr= getparameter(0);
        const double Estart=Estartptr->getvalue();
        const Parameter* Estopptr= getparameter(1);
        const double Estop=Estopptr->getvalue();
        double integral=0.0;
        double area=0.0;
        const double dispersion=this->getdispersion();
        const double Emax=this->getenergy(this->getnpoints()-1);
       // const double smoothvalue=500.0; //width of integration of the power law form the highest energy in the spectrum up to 500 eV above Estart

        for (size_t i=0;i<this->getnpoints();i++){
            const double en=this->getenergy(i);
            const double xsection=compptr->getcounts(i);         
            const double gdos=this->getcounts(i);
            //calc center of mass but only if in the integrationrange
            if (en<Estart+integrationwidth){
                integral+=en*gdos*xsection*dispersion;
                area+=gdos*xsection*dispersion;
            }
        }
        //extrapolate the pure cross section if the integrationrange lies outside of the spectrum range
        double A=0.0;
        double r=0.0;
        if (Estart+integrationwidth>Emax){
            //determine A and r
            fitpowerlaw(A,r,this->getenergyindex(Estop),this->getnpoints()-1); //fit power law from Estop till end
        }


        //make sure that Estop is not overlapping in this area
        //solve infinity issues at extrapolated power law by integrating only up to a certain energy
         const double integralextrapol1=-A*pow(Emax,2.0-r)/(2.0-r);
        const double areaextrapol1=-A*pow(Emax,1.0-r)/(1.0-r);
        const double integralextrapol2=-A*pow(Estart+integrationwidth,2.0-r)/(2.0-r);
        const double areaextrapol2=-A*pow(Estart+integrationwidth,1.0-r)/(1.0-r);
        const double integralextrapol=integralextrapol1-integralextrapol2;
        const double areaextrapol=areaextrapol1-areaextrapol2;

        const double mccorr=(integral+integralextrapol)/(area+areaextrapol);
        #ifdef COMPONENT_DEBUG
        std::cout <<" mccorr="<<mccorr<<"\n";
        #endif

        std::string msg;
        std::ostringstream s;

        if (s <<"Extrapolated Energy mass center of "<<this->getdisplayname()){ //converting an int to a string in c++ style rather than unsafe c-style
            msg=s.str();
        }
        mptr->setheaderstring(msg);
        mptr->setvalue(mccorr);                        
        return mccorr;
    }
return 0.0;
}


void GDos::fitpowerlaw(double&A,double&r,size_t startindex,size_t stopindex){   
    //do a power law fit and determine parameters A and r
    if (startindex>=stopindex){
        // a bad situation but take the 10 last pixels then
        startindex=stopindex-10;
    }
   const double N=double(stopindex)-double(startindex);
    //see Egerton p271
    double xy=0.0;
    double sx=0.0;
    double sy=0.0;
    double sxsq=0.0;
    for (size_t i=startindex;i<stopindex;i++){
        const double x=log(compptr->getenergy(i));
        const double y=log(compptr->getcounts(i)); //fit to the cross section!
        xy+=x*y;
        sx+=x;
        sy+=y;
        sxsq+=pow(x,2.0);
    }
    const double b=(N*xy-sx*sy)/(N*sxsq-pow(sx,2.0));
    const double a=sy/N-b*sx/N;
    r=-b;
    A=exp(a);



    #ifdef COMPONENT_DEBUG
    double yavgfit=0.0;
    double yavg=0.0;
    for (size_t i=startindex;i<stopindex;i++){
        const double en=compptr->getenergy(i);
        const double fit=A*pow(en,-r);
        yavgfit+=fit;
        yavg+=compptr->getcounts(i);
    }

    std::cout<<"power law fit A="<<A<<" r="<<r<<" yavg="<<yavg<<" yavgfit="<<yavgfit<<"\n";
    #endif

    //sanity check r<0
    if (r<0.0){
        r=3.0;
        A=0.0;
    }
    if (A<0.0){
        r=3.0;
        A=0.0;
    }
return;
}

void GDos::updatemonitors(){
        //calculate the mass center only here, not needed during normal calculation
        //const double mc=masscenter();

        //calculate edge onset by smoothing the finestructure and then tresholding
        this->smoothgaussian(smoothvalue); //smooth by 5eV
        size_t firstof=this->getfirsthigherthen(threshold);
        if (firstof<1){return;}
        //do linear interpolation to find subpixel onset     
        const double Eonsetlow=this->getenergy(firstof-1);
        const double levelhigh=this->getcounts(firstof);
        const double levellow=this->getcounts(firstof-1);
        const double levelrange=levelhigh-levellow;
        const double slope=levelrange/this->getdispersion();
        double dE=0.0;
        if (slope>1e-5){
            dE=(threshold-levellow)/slope;
        }
        const double Eonset=Eonsetlow+dE+3.0*smoothvalue; //treshold, correct for shifting over 3sigma due to filtering
              
        mptr=getparameter(3)->getmonitor();
        if (mptr!=0){
            //store the result in a monitor
             std::string msg;
            std::ostringstream s;
            if (s <<"Edge onset at "<<threshold*100.0<<"% of "<<this->getdisplayname()){ //converting an int to a string in c++ style rather than unsafe c-style
                msg=s.str();
            }
            mptr->setheaderstring(msg);
            mptr->setvalue(Eonset);
        }
        //calculate total relative area under fine structure
        double area=0.0;
        double atomicarea=0.0;
        const Parameter* Estartptr= getparameter(0);
        const double Estart=Estartptr->getvalue();
        const Parameter* Estopptr= getparameter(1);
        const double Estop=Estopptr->getvalue();              
        const double dispersion=this->getdispersion();
        
        for (size_t i=0;i<this->getnpoints();i++){
        	const double en=this->getenergy(i);
            const double xsection=compptr->getcounts(i);         
            const double gdos=this->getcounts(i);
            //calc area under atomic cross section as well as under equalised crystal cross section
            if ((en>Estart)&&(en<=Estop)){        	
                area+=gdos*xsection*dispersion;
                atomicarea+=xsection*dispersion;
            }
        }
        area=area/(compptr->getparameter(5))->getvalue();
        
        //also add monitor on param2
        mptr2=getparameter(2)->getmonitor();
        if (mptr2!=0){
        	//store the result in a monitor
            std::string msg;
            std::ostringstream s;
            if (s <<"Relative area under edge "<<this->getdisplayname()){ //converting an int to a string in c++ style rather than unsafe c-style
            	msg=s.str();
            }
            mptr2->setheaderstring(msg);
            mptr2->setvalue(area);
        }
                
        //undo the smoothing by recalculating this
        this->setchanged();
        this->calculate();
    }
