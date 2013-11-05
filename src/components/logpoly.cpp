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
 * eelsmodel - components/logpoly.cpp
 **/

#include "src/components/logpoly.h"

#include <cmath>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "src/core/parameter.h"

#include "src/gui/integerinput.h"

Logpoly::Logpoly() //create a dummy version
:Component()
{
  this->setname("Log polynomial background");
  this->setdescription("Log polynomial y=A*exp(a1*(E-Estart)+a2*|a2|(E-Estart)^2+...an*|an^n-1|(E-Estart)^n) with n+1 parameters");
  degree=0;
  setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
  setshifter(false);
}

Logpoly::Logpoly(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  if (parameterlistptr==0){
    //ask for the degree of the polynomial between 1 and 10
    const int min=1;
    const int max=10;
    int d=2;
    Integerinput myinput(0,"","enter degree of polynomial",d,min,max);
    degree=size_t(d);
    //create the parameters
    Parameter* p1=new Parameter("A",1.0e3,1);
    p1->interactivevalue("enter height at Estart");
    p1->setlinear(true);
    this->addparameter(p1);
    Parameter* p=0;
    //char cname[5];
    //strcpy(cname,"");
    std::string mesg;
    std::string name;
    for (size_t i=0;i<degree;i++){
      //sprintf(cname,"a%d",i+1);
      //name=cname;
      std::ostringstream s;
      if ((s << "a"<< i+1)){ //converting an int to a string in c++ style rather than unsafe c-style
	// conversion worked
	name=s.str();
      }
      try{
	p=new Parameter(name,-1.0e-1,1);
      }
      catch(...)
      {
	//allocation problem
	throw Componenterr::unable_to_create();
      }
      mesg="enter "+name;
      p->setboundaries(-1.0e-1,0.0);//must be negative
      p->interactivevalue(mesg);
      this->addparameter(p);
    }
  }
  else{
    degree=parameterlistptr->size()-1; //the number of parameters tells the degree+1 of the log poly
    //get parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
      Parameter* p=(*parameterlistptr)[i];
      if (i==1) p->setlinear(true); //A is a linear parameter
      this->addparameter(p);
    }
  }
  //give a name and description
  setname("Log polynomial background");
  setdescription("Log polynomial y=A*exp(a1*(E-Estart)+a2*|a2|(E-Estart)^2+...an*|an^n-1|(E-Estart)^n) with n+1 parameters");
  setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics

  //all parameters have an analytical gradient
  for (size_t i=0;i<degree+1;i++){//there are degree+1 parameters
  this->sethasgradient(i,true);
 }
  setshifter(false);
}
Logpoly::~Logpoly(){
}

void Logpoly::calculate()
{
  //check if any of parameters is changed, if not : don't calculate
  const Parameter* strengthptr= getparameter(0);
  const double strength=strengthptr->getvalue();
  Parameter* factorptr=0;
  bool changes=false;
  std::vector<double> A;
  A.resize(degree);
  for (size_t index=0;index<degree;index++){
    factorptr=getparameter(index+1);
    changes=(changes||factorptr->changed()); //see if any of the parameters is changed
    A[index]=factorptr->getvalue();
   }

  if (strengthptr->changed()||changes){
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating log poly \n strength: " << strength <<"\n";
    for (size_t index=0;index<degree;index++){
      std::cout<< " a" <<index<<" :"<<A[index]<< "\n";
    }
    #endif


    double en0=this->getenergy(0);
    for (unsigned int i=0;i<(this->getnpoints());i++){
      const double en=this->getenergy(i);
      const double x=en-en0;
      //do the first order term
      double cts=strength*exp(A[0]*x);
      //multiply by the higher order terms
      for (size_t index=1;index<degree;index++){
        cts*=exp(A[index]*pow(x,index+1));
      }
      this->setdatapoint(i,en,cts,0.0);
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

Logpoly* Logpoly::clone()const{
return new Logpoly(*this);}
Logpoly* Logpoly::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Logpoly(n,estart,dispersion,parameterlistptr);
}


Spectrum* Logpoly::getgradient(size_t j){
  //get analytical partial derivative to parameter j in point i
  const double en0=this->getenergy(0);
  const Parameter* strengthptr= getparameter(0);
  const double strength=strengthptr->getvalue();
  Parameter* factorptr=0;
  std::vector<double> A;
  A.resize(degree);
  for (size_t index=0;index<degree;index++){
      factorptr=getparameter(index+1);
      A[index]=factorptr->getvalue();
  }

  if (j==0){
    //analytical derivative wrt A
    for (size_t i=0;i<this->getnpoints();i++){
      const double en=this->getenergy(i);
      const double x=en-en0;
      double cts=exp(A[0]*x);
      //multiply by the higher order terms
      for (size_t index=1;index<degree;index++){
	cts*=exp(A[index]*pow(x,index+1));
      }
      gradient.setcounts(i,cts);
    }
  }


  if ((j>0)&&(j<=degree)){
    //analytical derivative wrt aj
    for (unsigned int i=0;i<(this->getnpoints());i++){
      const double en=this->getenergy(i);
      const double x=en-en0;
      const unsigned int n=j-1;
      double cts=pow(x,n+1)*strength*exp(A[0]*x);
      //multiply by the higher order terms
      for (size_t index=1;index<degree;index++){
        cts*=exp(A[index]*pow(x,index+1));
      }
    gradient.setcounts(i,cts);
    }
  }

  if (j>degree){
    //bad index
    throw Componenterr::bad_index();
  }
  return &gradient;
}
