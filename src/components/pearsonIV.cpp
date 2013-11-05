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
 * eelsmodel - components/pearsonIV.cpp
 **/

#include "src/components/pearsonIV.h"

#include <cmath>
#include <iostream>

#include "src/core/eelsmodel.h"
#include "src/core/parameter.h"

Eelsmodel* geteelsmodelptr();

PearsonIV::PearsonIV()
:Component()
{
  this->setname("Pearson IV");
  this->setdescription("Pearson IV peak (model for ZL peak)");
  setshifter(false);
}
PearsonIV::PearsonIV(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Parameter* p5;

  if (parameterlistptr==0){
    p1=new Parameter("Emax",0.0,1);
    p1->interactivevalue("enter position");

    p2=new Parameter("m",1.0,1);
    p2->setboundaries(0.5,1e5);
    p2->interactivevalue("m value (m=1 Cauchy,m=2 Lorentz, m=inf Gauss)");


    p3=new Parameter("nu",1.0,1);
    p3->interactivevalue("nu");

    p4=new Parameter("a",1.0,1);
    p4->setboundaries(0.0,1e10); //convention, let nu take the negative part since transform a:-a and nu:-nu leave function unchanged
    p4->interactivevalue("a");

    const Model* mymodel=geteelsmodelptr()->getmodel();
    const double maxval=mymodel->getHLptr()->getmax();
    p5=new Parameter("Height",maxval,1);
    p5->interactivevalue("enter height");
  }
  else{
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
    p3=(*parameterlistptr)[2];
    p4=(*parameterlistptr)[3];
    p5=(*parameterlistptr)[4];

  }

  //Height is a linear parameter
  p5->setlinear(true);


  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);
  this->addparameter(p5);

  //give a name and description
  this->setname("Pearson IV");
  this->setdescription("Pearson IV peak (model for ZL peak)");
  this->setcanconvolute(true);

  //all three parameters have an analytical gradient
  //TODO add numerical derivatives
  this->sethasgradient(0,false);
  this->sethasgradient(1,false);
  this->sethasgradient(2,false);
  this->sethasgradient(3,false);
  this->sethasgradient(4,true);


  setshifter(false); //seems to be really important here
}

PearsonIV::~PearsonIV(){
}

double PearsonIV::getarea()
{
//return the area under the pearsonIV peak
//TODO, get this numerically? (although problems with sampling might make this less acurate)
    double area=this->cumsum();
return area;
}

void PearsonIV::calculate()
{
//check if any parameter is changed, if not : don't calculate
const Parameter* lambdaptr= getparameter(0);
const double lambda=lambdaptr->getvalue();

const Parameter* mptr= getparameter(1);
const double m=mptr->getvalue();

const Parameter* nuptr= getparameter(2);
const double nu=nuptr->getvalue();

const Parameter* aptr= getparameter(3);
const double a=aptr->getvalue();

const Parameter* strengthptr= getparameter(4);
const double strength=strengthptr->getvalue();

if ((lambdaptr->changed())||(mptr->changed())||(aptr->changed())||(strengthptr->changed())||(nuptr->changed()))
{
  //if these have changed, do new calculation
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating pearsonIV \n lambda: " << lambda << " m:" <<m<<" nu:" <<nu<<" a:" <<a<<" strength: "<<strength<< "\n";
  #endif
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    const double en=this->getenergy(i);

    //const double cts=height*(pow(FWHM,2.0)/4.0) * ( 1.0/ ( pow((en-Epeak),2.0) + pow((FWHM/2.0),2.0) ) );
    const double cts=strength*pow(1.0+pow((en-lambda)/a,2.0),-m)*exp(-nu*atan((en-lambda)/a));
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

Spectrum* PearsonIV::getgradient(size_t j){
    //no analytical gradients yet
 return 0;
}


PearsonIV* PearsonIV::clone()const{
return new PearsonIV(*this);}
PearsonIV* PearsonIV::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new PearsonIV(n,estart,dispersion,parameterlistptr);
 }




