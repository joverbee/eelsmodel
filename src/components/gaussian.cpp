/***************************************************************************
                          gaussian.cpp  -  description
A gaussian peak component
                             -------------------
    begin                : Sun Oct 27 2002
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

//#define COMPONENT_DEBUG

#include "src/components/gaussian.h"

#include <cmath>
#include <iostream>
#include <valarray>

#include "src/core/parameter.h"

Gaussian::Gaussian()
:Component()
{
this->setname("Gaussian");
this->setdescription("Gaussian peak with given height,position and FWHM");
}
Gaussian::Gaussian(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
eps=1.0e-99; //to avoid divisions by zero
//add the required parameters
Parameter* p1;
Parameter* p2;
Parameter* p3;
if (parameterlistptr==0){
  p1=new Parameter("Emax",estart,1);
  p1->interactivevalue("enter position");
  p2=new Parameter("FWHM",10*dispersion,1);
  p2->interactivevalue("enter FWHM");
  p3=new Parameter("Height",1.0e3,1);
  p3->interactivevalue("enter height");
}
else{
  p1=(*parameterlistptr)[0];
  p2=(*parameterlistptr)[1];
  p3=(*parameterlistptr)[2];
}

p3->setlinear(true); //strength is a linear parameter
this->addparameter(p1);
this->addparameter(p2);
this->addparameter(p3);
//give a name and description
this->setname("Gaussian");
this->setdescription("Gaussian peak with given height,position and FWHM");
this->setcanconvolute(true);
//all three parameters have an analytical gradient
this->sethasgradient(0,true);
this->sethasgradient(1,true);
this->sethasgradient(2,true);

setshifter(false);
}

Gaussian::Gaussian(int n,double estart,double dispersion,double epeak,double fwhm,double height)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  p1=new Parameter("Emax",epeak,1);
  p2=new Parameter("FWHM",fwhm,1);
  p3=new Parameter("Height",height,1);
  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  //give a name and description
  this->setname("Gaussian");
  this->setdescription("Gaussian peak with given height,position and FWHM");
  this->setcanconvolute(true);
  this->sethasgradient(0,true);
  this->sethasgradient(1,true);
  this->sethasgradient(2,true);
  setshifter(false);
}



Gaussian::~Gaussian(){
}
void Gaussian::calculate()
{
//check if any parameter is changed, if not : don't calculate
const Parameter* Epeakptr= getparameter(0);
const double Epeak=Epeakptr->getvalue();
const Parameter* FWHMptr= getparameter(1);
const double FWHM=FWHMptr->getvalue();
const Parameter* heightptr= getparameter(2);
const double height=heightptr->getvalue();

if ((Epeakptr->changed())||(FWHMptr->changed())||(heightptr->changed()))
{
  //if these have changed, do new calculation
  double sigsq=pow(fabs(FWHM)/(2.0*sqrt(2.0*log(2.0))),2.0);
  if (fabs(sigsq)<eps){
     sigsq=eps;
  }
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating Gaussian \n Epeak: " << Epeak << " FWHM:" <<FWHM<<" height: "<<height<< "\n";
   std::cout << "the first energy points are: " << this->getenergy(0)<< " " <<this->getenergy(1)<<" "<<this->getenergy(2)<<"\n";


  #endif
  for (size_t i=0;i<this->getnpoints();i++)
  {
    const double en=this->getenergy(i);
    //arrea is FWHM*pi/2
    const double cts=height*exp(-pow(fabs(en-Epeak),2.0)/(2.0*sigsq));
    this->setdatapoint(i,en,cts,1.0);
  }
   #ifdef COMPONENT_DEBUG

std::cout << "the first data points are: " << this->getcounts(0)<< " " <<this->getcounts(1)<<" "<<this->getcounts(2)<<"\n";


  #endif
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
Spectrum* Gaussian::getgradient(size_t j){
//get analytical partial derivative to parameter j in point i
  const Parameter* Epeakptr= getparameter(0);
  double Epeak=Epeakptr->getvalue();
  const Parameter* FWHMptr= getparameter(1);
  double FWHM=FWHMptr->getvalue();
  const Parameter* heightptr= getparameter(2);
  double height=heightptr->getvalue();

  double sig=fabs(FWHM)/(2.0*sqrt(2.0*log(2.0)));
  double  sigsq=pow(sig,2.0);
  if (fabs(sigsq)<eps){
     sigsq=eps;
  }
  #ifdef COMPONENT_DEBUG
  std::cout << "calculating the partial derivative in Epeak: " << Epeak << " FWHM:" <<FWHM<<" height: "<<height<< "\n";
  #endif
  switch(j){
  case 0:
  //analytical derivative wrt Epeak
    for (size_t i=0;i<this->getnpoints();i++)
  {
      const double en=this->getenergy(i);
      gradient.setcounts(i,((en-Epeak)/sigsq)*height*exp(-pow(en-Epeak,2.0)/(2.0*sigsq)));
  }
  break;
  case 1:
  //analytical derivative wrt FWHM
   for (size_t i=0;i<(this->getnpoints());i++)
  {
      const double en=this->getenergy(i);
      gradient.setcounts(i,(pow((en-Epeak),2.0)/(sigsq*fabs(FWHM)))*height*exp(-pow(en-Epeak,2.0)/(2.0*sigsq)));
  }
  break;
  case 2:
  //analytical derivative wrt Height
   for (size_t i=0;i<(this->getnpoints());i++)
    {
      const double en=this->getenergy(i);
      gradient.setcounts(i,exp(-pow(en-Epeak,2.0)/(2.0*sigsq)));
    }

  break;
  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}
Gaussian* Gaussian::clone()const{
return new Gaussian(*this);}
Gaussian* Gaussian::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Gaussian(n,estart,dispersion,parameterlistptr);
 }


//as it was in the old C EELSMODEL
/*
int Gaussian(double dat[],double Estart,double Estep,int channels,double Epeak,double FWHM)
{
  //Gaussian function with peak height 1 calculated from Estart till Estop in Estep increments
  double E;
  double sig;
  int i;
  sig=FWHM/(2.0*sqrt(2.0*log(2.0)));
  for (i=0;i<channels;i++)
    {
      E=Estart+Estep*(double)i;
      dat[i]=exp(-pow(E-Epeak,2.0)/(2.0*pow(sig,2.0)));
   }
}
*/

