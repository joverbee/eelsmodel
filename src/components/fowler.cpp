/***************************************************************************
                          fowler.cpp  -  description
A Fowler Nordheim peak component to model a FEG zero loss peak
                             -------------------
    begin                : Mon May 5  2005
    copyright            : (C) 2005 by Jo Verbeeck
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

#include "src/components/fowler.h"

#include <cmath>
#include <iostream>

#include "src/core/parameter.h"

Fowler::Fowler()
:Component()
{
  this->setname("Fowler Nordheim");
  this->setdescription("Fowler Nordheim peak with given height,position, tunnel barier and temperature");
  setshifter(false);
}
Fowler::Fowler(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  if (parameterlistptr==0){
    p1=new Parameter("Emax",0.0,1);
    p1->interactivevalue("enter position [eV]");
    p2=new Parameter("Height",1.0e3,1);
    p2->interactivevalue("enter height");
    p3=new Parameter("d",4.5,1);
    p3->interactivevalue("enter tunnel barier [eV]");
    p4=new Parameter("T",2000,1);
    p4->interactivevalue("enter tip temperature [K]");
  }
  else{
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
    p3=(*parameterlistptr)[2];
    p4=(*parameterlistptr)[3];
  }
  p2->setlinear(true); //height is a linear parameter
  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);
  //give a name and description
  this->setname("Fowler Nordheim");
  this->setdescription("Fowler Nordheim peak with given height,position, tunnel barier and temperature");
  this->setcanconvolute(true);
  //only height has an analytical gradient
  this->sethasgradient(0,false);
  this->sethasgradient(1,true);
  this->sethasgradient(2,false);
  this->sethasgradient(3,false);
  setshifter(false);
}


Fowler::Fowler(int n,double estart,double dispersion,double epeak,double height,double d, double T)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  p1=new Parameter("Emax",epeak,1);
  p2=new Parameter("Height",height,1);
  p3=new Parameter("d",d,1);
  p4=new Parameter("T",T,1);
  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);

  //give a name and description
  this->setname("Fowler Nordheim");
  this->setdescription("Fowler Nordheim peak with given height,position, tunnel barier and temperature");
  this->setcanconvolute(true);
  //only height parameter has an analytical gradient
  this->sethasgradient(0,false);
  this->sethasgradient(1,true);
  this->sethasgradient(2,false);
  this->sethasgradient(3,false);
  setshifter(false);
}


Fowler::~Fowler(){
}

void Fowler::calculate()
{
//check if any parameter is changed, if not : don't calculate
const Parameter* Epeakptr= getparameter(0);
const double Epeak=Epeakptr->getvalue();
const Parameter* heightptr= getparameter(1);
const double height=heightptr->getvalue();
const Parameter* dptr= getparameter(2);
const double d=dptr->getvalue();
const Parameter* Tptr= getparameter(3);
const double T=Tptr->getvalue();
//const double kB=1.3806503e-23; //boltzmann constant J/K
const double kB=8.6173426e-5; //boltzmann constant eV/K
//const double e=1.602176462e-19; //charge of electron in C
const double eps=1.0e-3; //minimum decent value for d or T

if ((Epeakptr->changed())||(Tptr->changed())||(dptr->changed())||(heightptr->changed()))
{
  //if these have changed, do new calculation
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating Fowler Nordheim\n Epeak: " << Epeak << " height: "<<height<< " d: "<<d<<" T: "<<T<<"\n";
  #endif
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    const double en=this->getenergy(i);
    double Emax=0.0;
    if ((kB*T<d)&&(T>eps)&&(d>eps)){
      Emax=-kB*T*log(kB*T/(d*(1-kB*T/d))); //the maximum of the FN function, this is not at E=0!
    }
    const double dE=en-Epeak+Emax;
    const double norm=FNfunction(Emax,d,T);
    const double cts=height*FNfunction(dE,d,T)/norm;
    this->setcounts(i,cts);
  }
  //set parameters as unchanged since last time we calculated
  this->setunchanged();
}
else{
  //if nothing is changed
  #ifdef COMPONENT_DEBUG
  std::cout <<"parameters have not changed, i don't need to calculate again\n";
  #endif
  }
}

Spectrum* Fowler::getgradient(size_t j){
//get analytical partial derivative to parameter j in point i
const Parameter* heightptr= getparameter(1);
const double height=heightptr->getvalue();




  #ifdef COMPONENT_DEBUG
    const Parameter* dptr= getparameter(2);
    const Parameter* Tptr= getparameter(3);
    const Parameter* Epeakptr= getparameter(0);
    const double Epeak=Epeakptr->getvalue();
    const double d=dptr->getvalue();
    const double T=Tptr->getvalue();
    std::cout << "calculating the partial derivative in Epeak: " << Epeak << " height: "<<height<< " d: "<<d<<" T: "<<T<<"\n";
  #endif

  switch(j){

  case 1:
  //analytical derivative wrt height
   for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    gradient.setcounts(i,this->getcounts(i)/height);
  }
  break;
  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}


Fowler* Fowler::clone()const{
return new Fowler(*this);}
Fowler* Fowler::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Fowler(n,estart,dispersion,parameterlistptr);
 }
double Fowler::FNfunction(double energy, double d,double T)
{
  //Fowler Nordheim function, carefull, maximum is not at energy=0
  const double kB=8.6173426e-5; //boltzmann constant eV/K
  const double Tmin=300; //minimum decent value for temperature
  const double dmin=0.1; //minimum decent value for d
  if ((T>Tmin)&&(d>dmin)){
    return exp(-energy/d)/(1+exp(-energy/(kB*T)));}
  else{
    return 0.0;
  }
}
