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
 * eelsmodel - components/pseudovoigt.cpp
 **/

#include "src/components/pseudovoigt.h"

#include <cmath>
#include <iostream>

#include "src/components/lorentz.h"
#include "src/components/gaussian.h"

#include "src/core/parameter.h"

PseudoVoigt::PseudoVoigt()
:Component()
{
  this->setname("Pseudo-Voigt");
  this->setdescription("Pseudo-Voigt peak with given height,position and FWHM");
  setshifter(false);
  Lorentzptr=0;
  Gaussptr=0;
}
PseudoVoigt::PseudoVoigt(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  Lorentzptr=0;
  Gaussptr=0;
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Parameter* p5;
  if (parameterlistptr==0){
    p1=new Parameter("Emax",estart,1);
    p1->interactivevalue("enter position");
    p2=new Parameter("FWHM Lorentz",10*dispersion,1);
    p2->interactivevalue("enter FWHM Lorentz");
    p3=new Parameter("FWHM Gauss",10*dispersion,1);
    p3->interactivevalue("enter FWHM Gauss");
    p4=new Parameter("Lorentz/Gauss fraction",1.0,1);
    p4->interactivevalue("enter Lorentz/Gauss fraction");
    p5=new Parameter("Height",1.0e3,1);
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
  this->setname("Pseudo-Voigt");
  this->setdescription("Pseudo-Voigt peak with given height,position and FWHM");
  this->setcanconvolute(true);
  //all three parameters have an analytical gradient

  //this->sethasgradient(0,true);
  //this->sethasgradient(1,true);
  //this->sethasgradient(2,true);

  setshifter(false); //seems to be really important here

  //create Lorentzian with height 1
  Lorentzptr=new Lorentz(this->getnpoints(),this->getenergy(0),this->getdispersion(),p1->getvalue(),p2->getvalue(),1.0);
  //create Gaussian
  Gaussptr=new Gaussian(this->getnpoints(),this->getenergy(0),this->getdispersion(),p1->getvalue(),p2->getvalue(),1.0);

}




PseudoVoigt::~PseudoVoigt(){
    //clean up the Gauss and Lorentzian
    if (Gaussptr!=0) delete(Gaussptr);
    if (Lorentzptr!=0) delete(Lorentzptr);
}



void PseudoVoigt::calculate()
{
//check if any parameter is changed, if not : don't calculate
const Parameter* Epeakptr= getparameter(0);
const double Epeak=Epeakptr->getvalue();
const Parameter* LFWHMptr= getparameter(1);
const double LFWHM=LFWHMptr->getvalue();
const Parameter* GFWHMptr= getparameter(2);
const double GFWHM=GFWHMptr->getvalue();
const Parameter* etaptr= getparameter(3);
const double eta=etaptr->getvalue();
const Parameter* heightptr= getparameter(4);
const double height=heightptr->getvalue();

//if any of the parameters of the Lorentz or Gauss changed, then update them
//strength in not important for these, we keep them normalised
if ((LFWHMptr->changed())||(Epeakptr->changed())){

    Lorentzptr->getparameter(0)->setvalue(Epeak);
    Lorentzptr->getparameter(1)->setvalue(LFWHM);

    Lorentzptr->calculate();
}
if ((GFWHMptr->changed())||(Epeakptr->changed())){
    Gaussptr->getparameter(0)->setvalue(Epeak);
    Gaussptr->getparameter(1)->setvalue(GFWHM);

    Gaussptr->calculate();
}

//now assemble the pseudovoigt
if ((LFWHMptr->changed())||(Epeakptr->changed())||(GFWHMptr->changed())||(heightptr->changed())||(etaptr->changed())){
    for (size_t i=0;i<(this->getnpoints());i++){
        const double pvoigt=height*(eta*Lorentzptr->getcounts(i)+(1.0-eta)*Gaussptr->getcounts(i)); //eq 5 in H. Wang, J. Zhou, Aplied Cristallography, (2005), 38 830-832
        this->setcounts(i,pvoigt);
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

Spectrum* PseudoVoigt::getgradient(size_t j){
return 0; //no gradients available
}


PseudoVoigt* PseudoVoigt::clone()const{
return new PseudoVoigt(*this);}
PseudoVoigt* PseudoVoigt::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new PseudoVoigt(n,estart,dispersion,parameterlistptr);
 }


//as it was in the old C EELSMODEL
/*
int PseudoVoigt(double dat[],double Estart,double Estep,int channels,double Epeak,double FWHM)
{
  //PseudoVoigtian function with peak height 1 calculated from Estart till Estop in Estep increments
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


