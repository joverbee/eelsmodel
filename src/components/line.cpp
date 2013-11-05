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
 * eelsmodel - components/line.h
 **/

#include "src/components/line.h"

#include <cmath>
#include <iostream>

#include "src/core/parameter.h"

Line::Line()
:Component()
{
this->setname("Line");
this->setdescription("y=A+BE with A and B the two parameters");
}
Line::Line(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
//add the required parameters
Parameter* p1;
Parameter* p2;
if (parameterlistptr==0){
  p1=new Parameter("A",1.0e3,1);
  p1->interactivevalue("enter A for Y=A+BE");
  p2=new Parameter("B",1.0,1);
  p2->interactivevalue("enter B for Y=A+BE");
}
else{
  p1=(*parameterlistptr)[0];
  p2=(*parameterlistptr)[1];
}

//both paramters are linear parameters
p1->setlinear(true);
p2->setlinear(true);

this->addparameter(p1);
this->addparameter(p2);
//give a name and description
this->setname("Line");
this->setdescription("y=A+BE with A and B the two parameters");
this->setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
}
Line::~Line(){
}
void Line::calculate()
{
//check if any of paramters is changed, if not : don't calculate
const Parameter* p1= getparameter(0);
double A=p1->getvalue();
const Parameter* p2= getparameter(1);
double B=p2->getvalue();

if (p1->changed()||p2->changed())
{
    #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating power law \n A: " << A << " B:" <<B<< "\n";
  #endif
  for (unsigned int i=0;i<(this->getnpoints());i++)
  {
    double en=this->getenergy(i);
    double cts=A+B*en;
    this->setdatapoint(i,en,cts,0.0);
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

Line* Line::clone()const{
return new Line(*this);}
Line* Line::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Line(n,estart,dispersion,parameterlistptr);
 }
