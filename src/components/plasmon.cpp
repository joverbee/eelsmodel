/***************************************************************************
                          plasmon.cpp  -  description
a simple plasmon model to model the LL spectrum
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

#include "src/components/plasmon.h"
#include "src/components/lorentz.h"
#include "src/gui/integerinput.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>
#include <string>
#include <vector>



Plasmon::Plasmon() //create a dummy version
:Component()
{
  this->setname("Plasmon model");
  this->setdescription("A model of a lorentzian ZL peak and a set of lorentzian plasmon peaks");
  nrofplasmons=0;
  setcanconvolute(true);
  setshifter(false);
}

Plasmon::Plasmon(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  //ask for number of plasmons to calculate
  const int min=1;
  const int max=10;
  nrofplasmons=2;
  Integerinput myinput(0,"","enter degree the number of plasmons",nrofplasmons,min,max);
  //create the lorentz peaks for ZL and plasmons
  peaklist.resize(0);
  const double EZL=0.0; //the zl position
  const double fwhmZL=1.0; //the fwhm of the ZL peak
  const double heightZL=1.0e3; //the height of the ZL peak
  //create the ZL peak
  Lorentz* ZL=new Lorentz(n,estart,dispersion,EZL,fwhmZL,heightZL);
  peaklist.push_back(ZL);
  //create the plasmon peaks
  const double EP=15.0; //the zl position
  const double fwhmP=10.0; //the fwhm of the ZL peak
  const double tlambda=0.3; //the t/lambda thickness factor
  for (int i=1;i<=nrofplasmons;i++){
    //a poisson related height for the ith plasmon
    double height=heightZL*poissonfraction(tlambda,i);
    Lorentz* plasmon=new Lorentz(n,estart,dispersion,i*EP,fwhmP,height);
    peaklist.push_back(plasmon);
  }

  //add the required parameters

  if (parameterlistptr==0){
    //we have to create a new set of parameters

    //position of the ZL
    Parameter* p1=new Parameter("EZL",EZL,1);
    p1->interactivevalue("enter the ZL position");
    this->addparameter(p1);
    //fwhm ZL
    Parameter* p2=new Parameter("fwhmZL",fwhmZL,1);
    p2->interactivevalue("enter fwhm of the ZL");
    this->addparameter(p2);
    //ZL height
    Parameter* p3=new Parameter("heightZL",heightZL,1);
    p3->interactivevalue("enter height of the ZL");
    this->addparameter(p3);

    //plasmon energy
    Parameter* p4=new Parameter("EP",EP,1);
    p4->interactivevalue("enter plasmon energy");
    this->addparameter(p4);
    //plasmon fwhm
    Parameter* p5=new Parameter("fwhmP",fwhmP,1);
    p5->interactivevalue("enter plasmon fwhm");
    this->addparameter(p5);
    //t/lambda
    Parameter* p6=new Parameter("tlambda",tlambda,1);
    p6->interactivevalue("enter plasmon fwhm");
    this->addparameter(p6);
    //darknoise
    double dnoise=0.0;
    Parameter* p7=new Parameter("dnoise",dnoise,1);
    p7->interactivevalue("enter darknoise value");
    this->addparameter(p7);


  }
  else{
    //get parameters from a list
    Parameter* p1=(*parameterlistptr)[0];
    this->addparameter(p1);
    Parameter* p2=(*parameterlistptr)[1];
    this->addparameter(p2);
    Parameter* p3=(*parameterlistptr)[2];
    this->addparameter(p3);
    Parameter* p4=(*parameterlistptr)[3];
    this->addparameter(p4);
    Parameter* p5=(*parameterlistptr)[4];
    this->addparameter(p5);
    Parameter* p6=(*parameterlistptr)[5];
    this->addparameter(p6);
    Parameter* p7=(*parameterlistptr)[6];
    this->addparameter(p7);
}


  //give a name and description
  setname("Plasmon model");
  setdescription("A model of a lorentzian ZL peak and a set of lorentzian plasmon peaks");
  setcanconvolute(true);
  setshifter(false);
}

Plasmon::~Plasmon(){
  //delete the lorentzians that were created
  const int n=peaklist.size();
  for (int index=0;index<n;index++){
    Lorentz* lptr=peaklist[index];
    if (lptr!=0)    delete(lptr);
  }
}

void Plasmon::calculate()
{
//check if any of paramters is changed, if not : don't calculate

const Parameter* EZLptr= getparameter(0);
const double EZL=EZLptr->getvalue();

const Parameter* fwhmZLptr= getparameter(1);
const double fwhmZL=fwhmZLptr->getvalue();
const Parameter* heightZLptr= getparameter(2);
const double heightZL=heightZLptr->getvalue();

const Parameter* EPptr= getparameter(3);
const double EP=EPptr->getvalue();
const Parameter* fwhmPptr= getparameter(4);
const double fwhmP=fwhmPptr->getvalue();

const Parameter* tlambdaptr= getparameter(5);
const double tlambda=tlambdaptr->getvalue();

const Parameter* dnoiseptr= getparameter(6);
const double dnoise=dnoiseptr->getvalue();


const bool changes=EZLptr->changed()||fwhmZLptr->changed()||heightZLptr->changed()||EPptr->changed()||fwhmPptr->changed()||tlambdaptr->changed()||dnoiseptr->changed();

if (changes)
{
  #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating plasmon model again \n";
  #endif

  //const double en0=this->getenergy(0);
  //set parameters of all lorentz peaks
  //the ZL peak
  Parameter* p=peaklist[0]->getparameter(0);
  p->setvalue(EZL);
  p=peaklist[0]->getparameter(1);
  p->setvalue(fwhmZL);
  p=peaklist[0]->getparameter(2);
  p->setvalue(heightZL);
  peaklist[0]->calculate();
  const double ZLarea=peaklist[0]->getarea();


  //the plasmon peaks
  const int nrofpeaks=peaklist.size();
  for (int index=1;index<nrofpeaks;index++){
    Lorentz* lptr=peaklist[index];

    Parameter* posptr=lptr->getparameter(0);
    Parameter* fwhmptr=lptr->getparameter(1);
    Parameter* heightptr=lptr->getparameter(2);
    posptr->setvalue(EZL+EP*double(index)); //the plasmon position
    fwhmptr->setvalue(fwhmP);
    heightptr->setvalue(1.0); //set to one first to get area of a lorentz with these parameters
    lptr->calculate();
    const double area=lptr->getarea();
    //calculate height
    double height=ZLarea/(area*poissonfraction(tlambda,index));
    heightptr->setvalue(height); //now set to right height
    lptr->calculate();
  }

  //add them to get the total model
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
      //double en=this->getenergy(i);
      double cts=dnoise+peaklist[0]->getcounts(i);
      for (int index=1;index<nrofpeaks;index++){
        cts+=peaklist[index]->getcounts(i);
      }
      this->setcounts(i,cts);
  }


  //set parameters as unchanged since last time we calculated
  this->setunchanged();
}
else
{
  #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, i don't need to calculate again\n";
  #endif
}

}
Plasmon* Plasmon::clone()const{
return new Plasmon(*this);}
Plasmon* Plasmon::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Plasmon(n,estart,dispersion,parameterlistptr);
}



double Plasmon::poissonfraction(double tlambda,int n)const{
//calculate the poisson fraction of the nth plasmon with respect to the ZL peak
//depending on the thickness t/lambda
//carefull the result is a area fraction of the nth plasmon wrt to the area of the ZL
//calculate n!
int nfac=n;
  for (int i=1;i<n;i++){
    nfac*=(n-i);
  }
//fraction is 1/n! * (t/lambda)^n      see egerton p186 pn/p0
double fraction=(1.0/double(nfac))*pow(tlambda,double(n));
return 1.0/fraction;
}
