/***************************************************************************
                          doslifetime.cpp  -  description
Density of states multiplier
this component creates a density of states profile that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by a basis set of n Lorentzians taking into account the lifetime broadening
according to R.F. Egerton, Ultramicroscopy 2007 doi:10.1016/j.ultramic.2006.11.005
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

#include "src/components/doslifetime.h"
#include "src/gui/integerinput.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>
#include "src/gui/graph.h"
#include "src/core/eelsmodel.h"
#include "src/gui/saysomething.h"
#include "src/components/lorentz.h"
#include "src/components/gaussian.h"
#include "src/gui/componentselector.h"

#ifdef DEBUG
    #include "debug_new.h" //memory leak checker
#endif


//#define COMPONENT_DEBUG

class QWorkspace;
//get two global pointers from main
extern QWorkspace* getworkspaceptr();

extern Eelsmodel* geteelsmodelptr();//main.cpp contains this global function with a pointer to eelsmodel
DosLifetime::DosLifetime() //create a dummy version
:Component(),peaklist()
{
  compptr=0;
  this->setname("Fine Structure (DOS) with lifetime");
  this->setdescription("Fine Structure used in combination with a normal cross-section using Lifetime broadening as an extra prior knowledge");
  degree=0;
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);
}

DosLifetime::DosLifetime(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),peaklist()
{
  compptr=0;
  if (parameterlistptr==0){
    //ask for the degree of the polynomial between 1 and 100
    const int min=1;
    const int max=1024;
    int d=10;
    Integerinput myinput(0,"","enter number of basis vectors",d,min,max);
    degree=size_t(d);
    //enter Estart and Estop
    Parameter* p1=new Parameter("Estart",estart,1);
    p1->interactivevalue("Enter Estart");
    p1->setchangeable(false);
    this->addparameter(p1);

    Parameter* p2=new Parameter("Estop",estart,1);
    p2->interactivevalue("Enter Estop");
    p2->setchangeable(false);
    this->addparameter(p2);

    //get Zeff and mass density to calculate the lifetime broadening
    Parameter* p3=new Parameter("atomic diameter [nm]",0.4,1);
    p3->interactivevalue("Enter atomic diameter [nm]");
    p3->setchangeable(false);
    this->addparameter(p3);


    //create the Lorentzian basis sets and store them in a vector and init them to 1.0
    for (size_t i=0;i<degree;i++){
      std::string name;
      std::ostringstream s;
      if ((s << "a"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     name=s.str();
      }
      //create a new Lorenztian
      const double width=(p2->getvalue()-p1->getvalue())/degree;
      double Epos=estart;
      const double fwhm=1.0*width/2.0;
      const double height=1.0;

#ifdef USELORENTZ
      Lorentz* L=new Lorentz(n,estart,dispersion,Epos,fwhm,height);
#endif
#ifndef USELORENTZ
      Gaussian* L=new Gaussian(n,estart,dispersion,Epos,fwhm,height);
#endif


#ifdef COMPONENT_DEBUG
      std::cout <<"added a new Lorentzian with Epos:"<<Epos<<" fwhm:"<<fwhm<<" height:"<<height<<" the adress is "<<L<<"\n";
#endif
      L->calculate();
      peaklist.push_back(L);


      //store parameters to hold the strengths of the basis set L
      Parameter* p=new Parameter(name,1.0,1);
      p->setboundaries(-1.0,50.0);
      //copy this parameter
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
    degree=(parameterlistptr->size())-4;//there are 4 other parameters

    //create the lorentzians needed
     //create a new Lorenztian
      const double width=1.0;
      const double Epos=estart;
      const double fwhm=1.0*width/2.0;
      const double height=1.0;


     for (size_t i=0;i<degree;i++){
#ifdef USELORENTZ
      Lorentz* L=new Lorentz(n,estart,dispersion,Epos,fwhm,height);
#endif
#ifndef USELORENTZ
      Gaussian* L=new Gaussian(n,estart,dispersion,Epos,fwhm,height);
#endif

      #ifdef COMPONENT_DEBUG
      std::cout <<"added a new Lorentzian with Epos:"<<Epos<<" fwhm:"<<fwhm<<" height:"<<height<<" the adress is "<<L<<"\n";
#endif

      L->calculate();
      peaklist.push_back(L);


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
#ifdef COMPONENT_DEBUG
      std::cout <<"After linking to the cross section\n";
#endif
  }

} //end of else

  #ifdef COMPONENT_DEBUG
      std::cout <<"Setting the names etc\n";
#endif
  //give a name and description
  setname("Fine Structure (DOS) with lifetime");
  setdescription("Fine Structure used in combination with a normal cross-section using Lifetime broadening as an extra prior knowledge");
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);


  for (size_t i=0;i<degree;i++){
    //tell that we have a gradient for each point a_i
    this->sethasgradient(i+3,true);
      }

//#ifdef COMPONENT_DEBUG
    dummy=new Spectrum(n,estart,dispersion);
     dummy->setname("warped energy scale");
     std::cout <<"created dummy="<<dummy<<" \n"  ;
//#endif
  InitLorentzians(); //do energy warping, and calculate the basis set of warped lorentzians or gaussians

  //show the current DOS
  calculate();
  setvisible(true);

    //show an equaliser
    this->showequalizer();
}
DosLifetime::~DosLifetime(){
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
  //throw away the Lorentzians
  //delete the lorentzians that were created
  for (size_t index=0;index<degree;index++){
    #ifdef USELORENTZ
      Lorentz* lptr=peaklist[index];
#endif
#ifndef USELORENTZ
      Gaussian* lptr=peaklist[index];
#endif

    if (lptr!=0)    delete(lptr);
  }
  #ifdef COMPONENT_DEBUG
    if (dummy!=0) delete(dummy);
#endif
}

bool DosLifetime::checkxsection(){
  //check if the cross section component we are pointing to has still the same number
  //otherwise update the component number parameter
  Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
  const int cnr=mymodel->getcomponentindexbypointer(compptr);
  if (compptr==NULL){
    return false;
  }
  Parameter* cnrptr= getparameter(degree+3);
  if ((cnr==-1)&&(compptr!=NULL)){
    //component not found also kill ourselves...
    Saysomething mysay(0,"Error","DOS: the component we are pointing to seems to be not present");

    delete(this);
    //it's over
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


void DosLifetime::calculate()
{
  if (!checkxsection()) return; //if a problem with the cross section we are pointing to, stop calculating
  //get the parameters
  const Parameter* Estartptr= getparameter(0);
//  const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= getparameter(1);
  const double Estop=Estopptr->getvalue();
   const Parameter* aptr= getparameter(2);


  //calculate the DOS
  Parameter* pointptr=0; //pointer to a datapoint in the list
  bool changes=false;
  for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+3);
    changes=(changes||pointptr->changed()); //see if any of the data point parameters has changed
   }
  if ((Estartptr->changed()||Estopptr->changed())||aptr->changed()){
    InitLorentzians();
    changes=true;
  }
  if (changes){
    //if anything changed, we need to calculate, if not we leave without calculating
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating DOS \n degree: " << degree <<"\n";
    std::cout << "Estart: " << Estart <<"\n";
    std::cout << "Estop: " << Estop <<"\n";
    for (size_t index=0;index<degree;index++){
      std::cout<< " a" <<index<<" :"<<getparameter(index+3)->getvalue()<< "\n";
    }
    #endif

      //force a calculation of those lorentzians that are changed
      //and sum the result in This
    this->clear();
#ifdef COMPONENT_DEBUG
    std::cout <<"after clear, degree="<<degree<<"\n";
#endif
    for (size_t index=0;index<degree;index++){
      pointptr=getparameter(index+3);
      //copy it in This with a weight of ai
      for (size_t i=0; i<this->getnpoints();i++){
	      const double cts=peaklist[index]->getcounts(i);
	      this->setcounts(i,this->getcounts(i)+pointptr->getvalue()*cts);
      }
#ifdef COMPONENT_DEBUG
	std::cout <<"Copied Lorentzian "<<index<<" in This\n";
#endif
    }

    //set to 1 after Estop
    for (size_t i=0; i<this->getnpoints();i++){
        const double en=this->getenergy(i);
        if (en>Estop){
           this->setcounts(i,1.0);
        }
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

Spectrum* DosLifetime::getgradient(size_t j){
int jindex=(int)j-3;
if ((jindex<0)||(jindex>=(int)degree)){
    //don`t have a derivative for these
    throw Componenterr::bad_index();
    return &gradient;
}
//copy a selected basisset to the gradient spectrum
 for (size_t i=0; i<this->getnpoints();i++){
     gradient.setdatapoint(i,this->getenergy(i),(peaklist[jindex]->getcounts(i))*(compptr->getcounts(i)),1.0);
  }
  return &gradient;
}

double  DosLifetime::Lifetimebroadening(double E){
  //calculate Lifetime broadening in eV according to Egerton 2007
  //E is the energy in eV above the onset
  const double epsilon=E;
  const double h=4.13566733e-15; //eV/s
  //two input parameters
  const Parameter* aptr=getparameter(2);
  const double a=aptr->getvalue(); //atomic diameter in nm
  //#ifdef COMPONENT_DEBUG
  //  std::cout <<"Z="<<Z<<" rho="<<rho<<" \n";
  //#endif

  const double m0=9.10938188e-31; //electron mass in kg
  const double e=1.60217646e-19; //electron charge (C)
  const double m=m0;
  const double lambda=538.0*a*pow(fabs(epsilon),-2.0)+0.41*pow(a,3.0/2.0)*sqrt(fabs(epsilon));
  double v=0.1;
  if (epsilon>this->getdispersion()){
    v=sqrt(2.0*e*epsilon/m); //speed
  }


  const double tau=lambda*1e-9/v; //lifetime in seconds
  //#ifdef COMPONENT_DEBUG
  //std::cout <<"v="<<v<<" tau="<<tau<<" lambda"<<lambda<<" a="<<a<<" \n";
  //#endif
  const double pi=acos(-1.0);
  double dE=h/(2.0*pi*tau);
  //if dE lower than dispersion or E< Eonset
  if (dE<this->getdispersion()){
    dE=this->getdispersion();
  }
  if (E<=0){
    dE=this->getdispersion();
  }
  //#ifdef COMPONENT_DEBUG
  //std::cout <<"Returning a lifetime broadening of dE="<<dE<<"for energy E="<<E<<"\n";
  //#endif
  return dE;
}

void DosLifetime::Warp(Component* L){
#ifdef COMPONENT_DEBUG
    std::cout <<"Entering Warping loop\n";
#endif
  //scale the original energy of the Lorentz component to 0 at onset
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  double Ewarp=0.0;
  for (size_t bin=0;bin<this->getnpoints();bin++){
    const double E=this->getenergy(bin);
    const double dE=Lifetimebroadening(E-Estart);
    Ewarp=Ewarp+1.0/dE;
    L->setenergy(bin,Ewarp);
  }
  //now shift this scale to have 0 at onset and have the same value at the highest index
  size_t Eminid=0;
  for (size_t bin=0;bin<this->getnpoints();bin++){
    if (this->getenergy(bin)<=Estart){
      Eminid=bin;
    }
  }
#ifdef COMPONENT_DEBUG
  std::cout <<"Eminid="<<Eminid<<"\n";
#endif
  const double Eonset=L->getenergy(Eminid);
  const double Emax=this->getenergy(this->getnpoints()-1)-this->getenergy(Eminid);
  const double Ewarpmax=L->getenergy(this->getnpoints()-1);
#ifdef COMPONENT_DEBUG
  std::cout <<"Eonset="<<Eonset<<" Emax="<<Emax<<" Ewarpmax="<<Ewarpmax<<"\n";
#endif

  for (size_t bin=0;bin<this->getnpoints();bin++){
    double E=L->getenergy(bin)-Eonset;
    E=E*Emax/Ewarpmax;
    L->setenergy(bin,E);
  }
#ifdef COMPONENT_DEBUG
    std::cout <<"End of  Warping loop\n";
#endif
  return;
}

void DosLifetime::InitLorentzians(){
#ifdef COMPONENT_DEBUG
    std::cout <<"Initing Lorentzians\n";
#endif
  //do this at the start and whenever the Estart or Estop changes
  size_t Eminid=0;
  size_t Emaxid=0;
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= this->getparameter(1);
  const double Estop=Estopptr->getvalue();

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
  const double width=(Estop-Estart )/double(degree);
  const double fwhm=1.0*width/2.0;

  for (size_t i=0;i<degree;i++){
#ifdef COMPONENT_DEBUG
    std::cout <<"about to get peak:"<<i<<"\n";
#endif


#ifdef USELORENTZ
      Lorentz* L=peaklist[i];
#endif
#ifndef USELORENTZ
      Gaussian* L=peaklist[i];
#endif


#ifdef COMPONENT_DEBUG
    std::cout <<"Selecting Lorentzian["<<i<<"] with adress"<<L<<"\n";
#endif
    //Change energy scale on all lorentzians
    Warp(L); //warp the energy scale of the Lorentzian and force a recalculation
    //change the position (a linear distribution on the warped Energy axis
    //find the indices of Estart and Estop
    const double Epos=L->getenergy(Eminid)+(double(i+1)/(double(degree)))*(L->getenergy(Emaxid)-L->getenergy(Eminid));
    Parameter* const posptr=L->getparameter(0);
    posptr->setvalue(Epos);
    Parameter* const fwhmptr=L->getparameter(1);
    fwhmptr->setvalue(fwhm);

    L->calculate();
#ifdef COMPONENT_DEBUG
    std::cout <<"Calculated a warped Lorentzian with Epos="<<Epos<<"\n";
    std::cout <<"Basis set i="<<i<<"\n";
    std::cout <<"Getting the fwhmptr="<<L->getparameter(1)<<"\n";
    std::cout <<"Getting the fwhm="<<(L->getparameter(1))->getvalue()<<"\n";
#endif
      //set basis sets to zero outside their range
  for (size_t i=0; i<this->getnpoints();i++){


    const double en=this->getenergy(i);
    if (en<=Estart){
      L->setcounts(i,0.0);
    }
    if (en>Estop){
      L->setcounts(i,0.0);
    }
  }

  }
//#ifdef COMPONENT_DEBUG
if (peaklist[0]!=0){
  //plot lifetime
  std::cout <<"Plotting the lifetime function dummy="<<dummy<<"\n";
  for (size_t i=0;i<this->getnpoints();i++){
      dummy->setcounts(i,peaklist[0]->getenergy(i));
      }
   std::cout <<"after copying the energy function\n";
  dummy->display(getworkspaceptr());


  //plot the basis functions in 1 plot
   std::cout <<"Plotting the basis functions\n";
#ifdef USELORENTZ
      Lorentz* L=peaklist[0];
#endif
#ifndef USELORENTZ
      Gaussian* L=peaklist[0];
#endif

   L->setvisible(true);
   L->show();

   //add the other
   for (size_t i=1;i<degree;i++){
       if  ((size_t)((L->getgraphptr())->getnplots())<(i+1)){
           (L->getgraphptr())->addgraph(peaklist[i]);  //or else create
       }else{

            (L->getgraphptr())->updategraph(i,peaklist[i]); //if it already exists update
       }
       }
}
//#endif



}

DosLifetime* DosLifetime::clone()const{
  return new DosLifetime(*this);
}
DosLifetime* DosLifetime::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new DosLifetime(n,estart,dispersion,parameterlistptr);
}



