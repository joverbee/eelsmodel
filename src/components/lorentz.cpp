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
 * eelsmodel - components/lorentz.cpp
 **/

#include "src/components/lorentz.h"

#include <cmath>
#include <iostream>

#include "src/core/parameter.h"

Lorentz::Lorentz()
:Component()
{
  name = "Lorentz";
  this->setdescription("Lorentz peak with given height,position and FWHM");
  setshifter(false);
}
Lorentz::Lorentz(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
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

  //Height is a linear parameter
  p3->setlinear(true);


  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  //give a name and description
  name = "Lorentz";
  this->setdescription("Lorentz peak with given height,position and FWHM");
  this->setcanconvolute(true);
  //all three parameters have an analytical gradient
  this->sethasgradient(0,true);
  this->sethasgradient(1,true);
  this->sethasgradient(2,true);
  setshifter(false); //seems to be really important here
}


Lorentz::Lorentz(int n,double estart,double dispersion,double epeak,double fwhm,double height)
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
  name = "Lorentz";
  this->setdescription("Lorentz peak with given height,position and FWHM");
  this->setcanconvolute(true);
  //all three parameters have an analytical gradient
  this->sethasgradient(0,true);
  this->sethasgradient(1,true);
  this->sethasgradient(2,true);
  setshifter(false);
}


Lorentz::~Lorentz(){
}

double Lorentz::getarea()
{
//return the area under the lorentz peak
const Parameter* FWHMptr= getparameter(1);
double FWHM=FWHMptr->getvalue();
const Parameter* heightptr= getparameter(2);
 double height=heightptr->getvalue();
const double pi=acos(-1.0);
const double area=pi*FWHM*height/2.0;
return area;
}

void Lorentz::calculate()
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
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating lorentz \n Epeak: " << Epeak << " FWHM:" <<FWHM<<" height: "<<height<< "\n";
  #endif
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    const double en=this->getenergy(i);
    //arrea is FWHM*pi/2
    const double cts=height*(pow(FWHM,2.0)/4.0) * ( 1.0/ ( pow((en-Epeak),2.0) + pow((FWHM/2.0),2.0) ) );
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

Spectrum* Lorentz::getgradient(size_t j){
//get analytical partial derivative to parameter j in point i
  const Parameter* Epeakptr= getparameter(0);
  double Epeak=Epeakptr->getvalue();
  const Parameter* FWHMptr= getparameter(1);
  double FWHM=FWHMptr->getvalue();
  const Parameter* heightptr= getparameter(2);
  double height=heightptr->getvalue();

  #ifdef COMPONENT_DEBUG
  std::cout << "calculating the partial derivative in Epeak: " << Epeak << " FWHM:" <<FWHM<<" height: "<<height<< "\n";
  #endif
  switch(j){
  case 0:
  //analytical derivative wrt Epeak
    for (unsigned int i=0;i<this->getnpoints();i++)
  {
      double en=this->getenergy(i);
      double denom=pow((en-Epeak),2.0) + pow((FWHM/2.0),2.0);
      gradient.setcounts(i,((height*pow(FWHM,2.0))/2.0)*((en-Epeak)/pow(denom,2.0)));

  }
  break;
  case 1:
  //analytical derivative wrt FWHM
   for (unsigned int i=0;i<(this->getnpoints());i++)
  {
      double en=this->getenergy(i);
      double denom=pow((en-Epeak),2.0) + pow((FWHM/2.0),2.0);
      gradient.setcounts(i,0.5*(height)*FWHM*(1.0/denom-(pow(FWHM,2.0)/4.0)*(1.0/pow(denom,2.0))));
  }
  break;
  case 2:
  //analytical derivative wrt Height
   for (unsigned int i=0;i<(this->getnpoints());i++)
    {
      double en=this->getenergy(i);
      double denom=pow((en-Epeak),2.0) + pow((FWHM/2.0),2.0);
      gradient.setcounts(i,(pow(FWHM,2.0)/4.0) * ( 1.0/ denom ));
    }

  break;
  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}


Lorentz* Lorentz::clone()const{
return new Lorentz(*this);}
Lorentz* Lorentz::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Lorentz(n,estart,dispersion,parameterlistptr);
 }


//as it was in the old C EELSMODEL
/*
int Lorentz(double dat[],double Estart,double Estep,int channels,double Epeak,double FWHM)
{
  //Lorentzian function with peak height 1 calculated from Estart till Estop in Estep increments
  double E;
  int i;
  for (i=0;i<channels;i++)
    {
      E=Estart+Estep*(double)i;
      dat[i]=(pow(FWHM,2.0)/4.0) * ( 1/ ( pow((E-Epeak),2.0) + pow((FWHM/2),2.0) ) );
//arrea = fwhm*pi/2
   }
}
*/


