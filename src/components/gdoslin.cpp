/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - components/GDoslin.cpp
 **/

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

#include "src/components/gdoslin.h"

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

GDoslin::GDoslin() //create a dummy version
:Component(),Evector(),Yvector(),b(),c(),d()
{
  cumsumrule=0;
  this->setname("Linear Fine Structure (DOS)");
  this->setdescription("Linear Fine Structure used to alter atomic cross sections in the near edge region");
  degree=0;
  setcanconvolute(true);
  setshifter(false);
  acc=0;
  sp=0;
  Plotspec=0;
  smoothvalue=5.0;
  threshold=0.25;
}

GDoslin::GDoslin(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),Evector(),Yvector(),b(),c(),d()
{
    acc=0;
    sp=0;
    Plotspec=0;
    realSUB=0;
    fourierSUB=0;
    real=0;
    fourier=0;
    dosumrule=false;
    cumsumrule=0;
    broadeningtype=CONSTANT;
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

    Parameter* p2=new Parameter("Ewidth",50.0,1);
    p2->interactivevalue("Energy width onset");
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
      p->setboundaries(-1.0e5,1e5);
      //p->setlinear(true); //these parameters are linear: not true as they are multiplied to another component
      this->addparameter(p);
    }
  }
  else{
    //get parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
      Parameter* p=(*parameterlistptr)[i];
        if (i>3) p->setlinear(true); //the points are linear
      this->addparameter(p);
    }
    degree=(parameterlistptr->size())-4;//there are 4 other parameters
    //get options from the options code
    makeoptions();
  }



  //give a name and description
     this->setname("Linear Fine Structure (DOS)");
     this->setdescription("Linear Fine Structure used to alter atomic cross sections in the near edge region");
  setcanconvolute(true);
  setshifter(false);

    //create info for the subspace buffer that contains a mirrored version of the subsample GDoslin
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

GDoslin::~GDoslin(){
  //delete the FFTW buffers
  if (realSUB!=0) FFTWdelete(realSUB);
  if (fourierSUB!=0) FFTWdelete(fourierSUB);
  if (real!=0) FFTWdelete(real);
  if (fourier!=0) FFTWdelete(fourier);

  //killing the spectrum automatically cleans up the graph
  //clean up the gsl stuff
  if (sp!=0) gsl_spline_free (sp);
  if (acc!=0) gsl_interp_accel_free (acc);
  if (Plotspec!=0) delete(Plotspec);

}



void GDoslin::calculate()
{

  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Ewidthptr= getparameter(1);
  const double Estop=Estart+Ewidthptr->getvalue();

  //calculate the DOS
  Parameter* pointptr=0; //pointer to a datapoint in the list
  bool changes=false;
  for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+4);
    changes=(changes||pointptr->changed()); //see if any of the data point parameters has changed
   }

  if (Estartptr->changed()||changes||Ewidthptr->changed()){


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
    if ((Estartptr->changed()||Ewidthptr->changed())||aptr->changed()){
        //setup a new energy grid
        initDOS();
    }
    //copy the parameter values in the spline vectors

    const double scale=1.0/( double(2*nsubspace));
    switch(interpolationtype){
        case 1:
            //linear type
            for (size_t i=0;i<(this->getnpoints());i++){
                const double en=this->getenergy(i);
                //do a linear interpolation to the data points

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
                        cts=(Ajindex)*fabs(en-Estart)/edistance;
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
                            //last interval stay constant till Estop
                            const double edistance=Estop-Evector[jindex+offset];

                            const double Ajindex=getparameter(jindex+4)->getvalue();
                            cts=Ajindex;//+(0.0-Ajindex)*epos/edistance;
                        }
                        else{
                            cts=0.0;
                        }
                    }
                    }
                }
                else{
                    cts=0.0;
                }
                this->setcounts(i,cts);
                //setdatapoint(i,en,cts,0.0); don't write the energy, this causes trouble with getenergy in combination with eshift
            }
            break;
        case 2:
            //cubic spline type
            copyparameters();
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
                    cts=0.0;
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
                        realSUB[i]=0.0;
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



    //set parameters as unchanged since last time we calculated
    this->setunchanged();

  }
  else{
    #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, i don't need to calculate again\n";
    #endif
  }
}

Spectrum* GDoslin::getgradient(size_t j){
  //get analytical partial derivative to parameter j and return pointer to spectrum containing the gradient
	
	
	//TODO check gradients, they seem to cause a memory allocation problem in the fitter
	


  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Ewidthptr= getparameter(1);
  const double Estop=Estart+Ewidthptr->getvalue();

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
        gradient.setcounts(i,((en-eprevious)/edistance));
	  }
	  else{
        gradient.setcounts(i,(1.0-(en-ej)/edistance) );
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

GDoslin* GDoslin::clone()const{
  return new GDoslin(*this);
}
GDoslin* GDoslin::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new GDoslin(n,estart,dispersion,parameterlistptr);
}

void GDoslin::rescaleavg(){
    //scale parameters to avg, this is handy if you do the calibration to the sum rule
    //since otherwise the scaling has no true meaning and the parameters start to drift off towards the boundaries
Parameter* pointptr=0;
    //determine avg
    double avg=0.0;
    for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+3);
    avg+=pointptr->getvalue();
   }
   avg=avg/degree;
   //apply it to the points
   for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+3);
    const double newval=pointptr->getvalue()/avg;
    pointptr->setvalue(newval);
   }

}
void GDoslin::preparesubspace(){
    //create a subsampled space, do this whenever Estart or Estop changes
    //we could easily cope with changing number of parameters
    //but the only question is how to do this in the user interface???

  //total number of points in this space
    const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Ewidthptr= getparameter(1);
    const double Estop=Estart+Ewidthptr->getvalue();
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

void GDoslin::setoptions(){
    //call a function to set the options of the DOS
    std::string optionstring=interactiveoptions();
    storeoptions(optionstring);
    initDOS();
    //force an update of the model
    Parameter* firstptr= getparameter(3);
    firstptr->forcechanged(); //otherwise the model doesn't want to calculate
    geteelsmodelptr()->componentmaintenance_doupdate(); //redraw the parameters in componentmaintenance
    geteelsmodelptr()->componentmaintenance_updatescreen();
    geteelsmodelptr()->slot_componentmaintenance_updatemonitors();//make sure the monitors are recalculated
}

void GDoslin::initDOS(){
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
    //tell that we have gradients for the GDoslin points
    //parameters 4....degree+4
    for (size_t i=4;i<(degree+4);i++){
    	//this->sethasgradient(i,(broadeningtype==CONSTANT)&&(interpolationtype==1));
    	//gradients don't seem to work, they cause a memory alocation problem????
    	this->sethasgradient(i,false);
    }    
}

std::string GDoslin::interactiveoptions(){
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
double GDoslin::getoptions(){
     //create a coded options code to store in a parameter
     double options=0;
     if (dosumrule){
         options=double(interpolationtype)+threshold;
    }else{
        options=-double(interpolationtype)-threshold;
    }
return options;
}
void GDoslin::storeoptions(std::string optionstring){
            Parameter* optionptr= getparameter(2);
            optionptr->setchangeable(true);
            optionptr->setvalue(getoptions());
            optionptr->setname(optionstring);
            optionptr->setchangeable(false);
}
void GDoslin::makeoptions(){
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

 void GDoslin::changedegree(size_t olddegree,size_t degree){
     //change number of degrees
     if (degree>olddegree){
         //add new points at end if olddegree<degree
         for (size_t i=0;i<(degree-olddegree);i++){
            std::string name;
            std::ostringstream s;
            if ((s << "a"<< i+olddegree)){ //converting an int to a string in c++ style rather than unsafe c-style
                name=s.str();
            }
            Parameter* p=new Parameter(name,1.0,1);
            p->setboundaries(-1.0e5,1.0e5);
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
    //tell the model that something changed
    //this kills the stored values however
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    mymodel->resetstorage();
    this->showequalizer(); //redraw the equaliser with new number of sliders
}
double  GDoslin::Lifetimebroadening(double E){
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
    const Parameter* Ewidthptr= getparameter(1);
    const double Estop=Estart+Ewidthptr->getvalue();
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
void GDoslin::InitEnergy(){
#ifdef COMPONENT_DEBUG
    std::cout <<"Initialising Energy points\n";
#endif
  //do this at the start and whenever the Estart or Estop changes
  size_t Eminid=0;
  size_t Emaxid=0;
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  Parameter* Ewidthptr= this->getparameter(1);
  double Estop=Estart+Ewidthptr->getvalue();

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

    //add a last point that is 1 at Estop
    Yvector.push_back(0.0);
    Evector.push_back(Estop);
    //a second last point to make sure the end of the spline aproaches the tail with a flat section
    if (offset==2){
        Yvector.push_back(0.0);
        Evector.push_back(Estop+1.0);
    }

#ifdef COMPONENT_DEBUG
std::cout <<"Evector\n";
for (size_t i=0;i<Evector.size();i++){
  std::cout <<"Evector[ "<<i<<"]="<<Evector[i]<<"\n";
}
#endif

}

void GDoslin::copyparameters(){
    //copy the paramters values in the Yvector
    for (size_t index=0;index<degree;index++){
        //copy the parameter values in a vector, remember that point Yvector[0] is the first point but not connected to the parameters
        Yvector[index+offset]=((this->getparameter(index+4))->getvalue());
    }
}
void GDoslin::dospline(){
    //calculate the spline coefficients
     gsl_spline_init (sp, &Evector[0], &Yvector[0], Evector.size());
}

double GDoslin::seval(double x){
    return gsl_spline_eval (sp, x, acc);
}


void GDoslin::gslinit(){
    //init a cubic spline
    acc= gsl_interp_accel_alloc ();
    sp= gsl_spline_alloc (gsl_interp_cspline, Evector.size());
}
void GDoslin::initplotspec(){
    //copy Yvector and Evector in plotspec
    if (Plotspec!=0){
        for (size_t i=0;i<Plotspec->getnpoints();i++){
            Plotspec->setdatapoint(i,Evector[i],Yvector[i],0.0);
        }

        (this->getgraphptr())->updategraph(1,Plotspec);

    }
}

//inherit show but also redefine what should happen with the dos window
void GDoslin::show(){
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








