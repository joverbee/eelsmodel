/***************************************************************************
                          expbackground.cpp  -  description
Exponential background component
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

#include "src/components/expbackground.h"

#include <cmath>
#include <iostream>

#include "src/core/parameter.h"

Expbackground::Expbackground() //create a dummy version
:Component()
{
this->setname("Exponential background");
this->setdescription("Exponential background");
}
Expbackground::Expbackground(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
//add the required parameters
Parameter* p1;
Parameter* p2;
if (parameterlistptr==0){
  p1=new Parameter("A",1.0e3,1);
  p1->interactivevalue("enter A for Y=Aexp(-alpha.E)");
  p2=new Parameter("alpha",1.00001,1);
  p2->interactivevalue("enter alpha for Y=Aexp(-alpha.E)");
}
else{
//use parameters from a list
  p1=(*parameterlistptr)[0];
  p2=(*parameterlistptr)[1];
}
  this->addparameter(p1);
  this->addparameter(p2);
//give a name and description
this->setname("Exponential background");
this->setdescription("Exponential background");
this->setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
setshifter(false);
}
Expbackground::~Expbackground(){
}
void Expbackground::calculate()
{
//check if any of paramters is changed, if not : don't calculate
const Parameter* p1= getparameter(0);
double A=p1->getvalue();
const Parameter* p2= getparameter(1);
double alpha=p2->getvalue();

if (p1->changed()||p2->changed())
{
  std::cout << "parameters changed calculating exponential \n A: " << A << " alpha:" <<alpha<< "\n";
  double cts=0;
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    double en=this->getenergy(i);
    if (en>0.0) cts=A*exp(-en*alpha);
    else cts=A;
    this->setdatapoint(i,en,cts,0.0);
  }
  //set parameters as unchanged since last time we calculated
  this->setunchanged();
}
else
{
std::cout <<"parameters have not changed, i don't need to calculate again\n";
}

}
Expbackground* Expbackground::clone()const{
return new Expbackground(*this);}
Expbackground* Expbackground::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Expbackground(n,estart,dispersion,parameterlistptr);
}


//as it was in old C EELSMODEL
/*
int Background_exp_model(double BG[],double Estart,double Estep,int channels,double alpha,double A )
{
  double E;
  int i;

  for (i=0;i<channels;i++)
    {
      E=Estart+Estep*(double)i;
      if (E>0.0)
	{
	  BG[i]=A*exp(-E*alpha);
	}
      else
	{
	  BG[i]=A;
	}
    }
}
*/
