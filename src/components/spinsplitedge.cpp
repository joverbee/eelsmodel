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
 * eelsmodel - components/spinsplitedge.cpp
 **/

#include "src/components/spinsplitedge.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <string>

#include "src/components/hsedge.h"
#include "src/components/gaussian.h"

#include "src/core/parameter.h"
#include "src/core/curvematrix.h"

#include "src/gui/getfilename.h"
#include "src/gui/imagedisplay.h"

SpinsplitEdge::SpinsplitEdge() //create a dummy version
:Component(),tempspectrum(){
  this->setname("Spin split edge section");
  this->setdescription("An edge consisting of two hartree slater cross sections and two lorentzian white lines, can be an L2,3 or M4,5 edge");
  //init the pointers
  continuum1=0;
  continuum2=0;
  whiteline1=0;
  whiteline2=0;
}

SpinsplitEdge::SpinsplitEdge(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion){
  //init the pointers
  continuum1=0;
  continuum2=0;
  whiteline1=0;
  whiteline2=0;
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Parameter* p5;
  Parameter* p6;
  Parameter* p7;
  Parameter* p8;
  Parameter* p9;
  Parameter* p10;
  Parameter* p11;
  Parameter* p12;
  if (parameterlistptr==0){
    p1=new Parameter("E0",300e3,1);
    p1->interactivevalue("enter primary energy [eV]");
    p1->setchangeable(false);  //the fitter should not change this in normal operation

    p2=new Parameter("Ek",estart,1);
    p2->interactivevalue("enter edge energy [eV]");

    //load the HSGOS table, store the filename in dummy parameter
    Getfilename myfile(0,"Select a HS-GOS file");
    myfile.setdirname("~");
    myfile.setfilter("GOS tables (*.*)");
    myfile.setcaption("Select a HS-GOS file");
    filename=myfile.open();
    if (filename=="") throw Componenterr::unable_to_create();//cancelled by user
    #ifdef COMPONENT_DEBUG
      std::cout <<"the filename is: "<<filename <<"\n";
    #endif
    p3=new Parameter(filename,0.0,1);
    p3->setchangeable(false);  //the fitter should not change this


    p5=new Parameter("colection angle",2.0e-3,1);
    p5->interactivevalue("enter collection half angle theta [rad]");
    p5->setchangeable(false); //the fitter should not change this in normal operation

    p4=new Parameter("thetasteps",100.0,1);
    p4->interactivevalue("enter number of steps for theta integration");
    p4->setchangeable(false); //the fitter should not change this in normal operation

    p6=new Parameter("strength",1.0e2,1);
    p6->interactivevalue("enter strength of edge");

    p7=new Parameter("Esplit",1.0e1,1);
    p7->interactivevalue("enter spin splitting energy");

    p8=new Parameter("Ratio",0.5,1);
    p8->interactivevalue("enter occupancy ratio between both levels");

    p9=new Parameter("whiteline_strength",1.0e2,1);
    p9->interactivevalue("enter strength of first white line");

    p10=new Parameter("fwhm",1.0,1);
    p10->interactivevalue("enter fwhm of white line");

    p11=new Parameter("energy gap start",0.0,1);
    p11->interactivevalue("enter start of second d-band gap");

    p12=new Parameter("energy gap end",0.0,1);
    p12->interactivevalue("enter end of second d-band gap");
  }
  else{
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
    p3=(*parameterlistptr)[2];
    p4=(*parameterlistptr)[3];
    p5=(*parameterlistptr)[4];
    p6=(*parameterlistptr)[5];
    p7=(*parameterlistptr)[6];
    p8=(*parameterlistptr)[7];
    p9=(*parameterlistptr)[8];
    p10=(*parameterlistptr)[9];
    //if p11 does not exist, create a zero p11, to be compatible with older version without gap
    if (parameterlistptr->size()<11){
        p11=new Parameter("energy gap start",0.0,1);
        parameterlistptr->push_back(p11);
        p12=new Parameter("energy gap end",0.0,1);
        parameterlistptr->push_back(p12);

    }
    else{
       p11=(*parameterlistptr)[10];
       p12=(*parameterlistptr)[11];
    }

    #ifdef COMPONENT_DEBUG
    std::cout <<"creating a HS with paramterpointer "<<parameterlistptr<<" \n";
    std::cout <<"p1 "<<p1->getname()<<" \n";
    std::cout <<"p2 "<<p2->getname()<<" \n";
    std::cout <<"p3 "<<p3->getname()<<" \n";
    std::cout <<"p4 "<<p4->getname()<<" \n";
    std::cout <<"p5 "<<p5->getname()<<" \n";
    std::cout <<"p6 "<<p6->getname()<<" \n";
    std::cout <<"p7 "<<p7->getname()<<" \n";
    std::cout <<"p8 "<<p8->getname()<<" \n";
    std::cout <<"p9 "<<p9->getname()<<" \n";
    std::cout <<"p10 "<<p10->getname()<<" \n";
    std::cout <<"p11 "<<p11->getname()<<" \n";
    std::cout <<"p12 "<<p12->getname()<<" \n";
    #endif
    filename=p3->getname();
  }

  p6->setlinear(true);
  p8->setlinear(true);
  p9->setlinear(true);

  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);
  this->addparameter(p5);
  this->addparameter(p6);
  this->addparameter(p7);
  this->addparameter(p8);
  this->addparameter(p9);
  this->addparameter(p10);
  this->addparameter(p11);
  this->addparameter(p12);


  //create the components
  const double E1=p2->getvalue();
  const double E2=p2->getvalue()+p7->getvalue();
  const double strength=p7->getvalue();
  //const double strength2=strength1*p8.getvalue();




  const double wstrength1=p9->getvalue();
  const double wstrength2=wstrength1*p8->getvalue();
  const double fwhm=p10->getvalue();
  //const double gapstart=p11.getvalue();
  //const double gapend=p12.getvalue();
  const double ratio=p8->getvalue();

  //create first continuum part
  std::vector<Parameter*> cont1par;

  Parameter* p1_1=new Parameter(*p1);
  Parameter* p1_2=new Parameter(*p2);
  Parameter* p1_3=new Parameter(*p3);
  Parameter* p1_4=new Parameter(*p4);
  Parameter* p1_5=new Parameter(*p5);
  Parameter* p1_6=new Parameter(*p6);

  cont1par.push_back(p1_1);
  cont1par.push_back(p1_2);
  cont1par.push_back(p1_3);
  p5->setchangeable(true); //make sure "this" can still change the value stored in p5
  p4->setchangeable(true); //make sure "this" can still change the value stored in p5
  //this does not affect the user accessible parameters of this because they are already added to the paramlist
  cont1par.push_back(p1_4);
  cont1par.push_back(p1_5);
  cont1par.push_back(p1_6);
  const double S1=strength/(1.0+ratio);
  Parameter* pS1=new Parameter("Strength1",S1,1);
  cont1par.push_back(pS1);
  //set all parameters as unbound
  (cont1par[0])->setbound(false);
  (cont1par[1])->setbound(false);
  (cont1par[2])->setbound(false);
  (cont1par[3])->setbound(false);
  (cont1par[4])->setbound(false);
  (cont1par[5])->setbound(false);
  (cont1par[6])->setbound(false);
  //set all parameters as changeable
  (cont1par[0])->setchangeable(true);
  (cont1par[1])->setchangeable(true);
  (cont1par[2])->setchangeable(true);
  (cont1par[3])->setchangeable(true);
  (cont1par[4])->setchangeable(true);
  (cont1par[5])->setchangeable(true);
  (cont1par[6])->setchangeable(true);

  continuum1=new HSedge(n,estart,dispersion,&cont1par);

  #ifdef COMPONENT_DEBUG
  std::cout<<"continuum1 ptr="<<continuum1<<"\n";
  #endif
  //create second continuum part,same as continuum1 but add spin split to Ek
  std::vector<Parameter*> cont2par;

  Parameter* p2_1=new Parameter(*p1);
  Parameter* p2_2=new Parameter(*p2);
  p2_2->setvalue(p2->getvalue()+p7->getvalue());
  Parameter* p2_3=new Parameter(*p3);
  Parameter* p2_4=new Parameter(*p4);
  Parameter* p2_5=new Parameter(*p5);
  Parameter* p2_6=new Parameter(*p6);

  cont2par.push_back(p2_1);
  cont2par.push_back(p2_2);
  cont2par.push_back(p2_3);
  cont2par.push_back(p2_4);
  cont2par.push_back(p2_5);
  cont2par.push_back(p2_6);
  const double S2=strength*ratio/(1.0+ratio);
  Parameter* pS2=new Parameter("Strength2",S2,1);
  cont2par.push_back(pS2);
  //set all parameters as unbound
  (cont2par[0])->setbound(false);
  (cont2par[1])->setbound(false);
  (cont2par[2])->setbound(false);
  (cont2par[3])->setbound(false);
  (cont2par[4])->setbound(false);
  (cont2par[5])->setbound(false);
  (cont2par[6])->setbound(false);
  //set all parameters as changeable
  (cont2par[0])->setchangeable(true);
  (cont2par[1])->setchangeable(true);
  (cont2par[2])->setchangeable(true);
  (cont2par[3])->setchangeable(true);
  (cont2par[4])->setchangeable(true);
  (cont2par[5])->setchangeable(true);
  (cont2par[6])->setchangeable(true);

  continuum2=new HSedge(n,estart,dispersion,&cont2par);

  #ifdef COMPONENT_DEBUG
  std::cout<<"continuum2 ptr="<<continuum2<<"\n";
  #endif
  //create first white line
  whiteline1=new Gaussian(n,estart,dispersion,E1,fwhm,wstrength1);
  #ifdef COMPONENT_DEBUG
  std::cout<<"whiteline1 ptr="<<whiteline1<<"\n";
  #endif
  //create second white line


  whiteline2=new Gaussian(n,estart,dispersion,E2,fwhm,wstrength2);
  #ifdef COMPONENT_DEBUG
  std::cout<<"whiteline2 ptr="<<whiteline2<<"\n";
  #endif
  //give a name and description
  this->setname("Spin split edge section");
  this->setdescription("An edge consisting of two hartree slater cross sections and two lorentzian white lines, can be an L2,3 or M4,5 edge");
   this->setcanconvolute(true);
   this->setshifter(false);

  if (continuum1==0||continuum2==0||whiteline1==0||whiteline2==0) throw Componenterr::unable_to_create();
}


SpinsplitEdge::~SpinsplitEdge(){
  //clean up the components which we own
  if (continuum1!=0)    delete(continuum1);
  if (continuum2!=0)    delete(continuum2);
  if (whiteline1!=0)    delete(whiteline1);
  if (whiteline2!=0)    delete(whiteline2);
}

void SpinsplitEdge::calculate(){
  //get the parameters
  const Parameter* E0ptr= getparameter(0);
  const double E0=E0ptr->getvalue();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  const Parameter* thetastepptr= getparameter(3);
  const double thetasteps=thetastepptr->getvalue();
  const Parameter* thetamaxptr= getparameter(4);
  const double thetamax=thetamaxptr->getvalue();
  const Parameter* strengthptr= getparameter(5);
  const double strength=strengthptr->getvalue();
  const Parameter* Esplitptr= getparameter(6);
  const double Esplit=Esplitptr->getvalue();
  const Parameter* ratioptr= getparameter(7);
  const double ratio=ratioptr->getvalue();
  const Parameter* wstrengthptr= getparameter(8);
  const double wstrength=wstrengthptr->getvalue();
  const Parameter* fwhmptr= getparameter(9);
  const double fwhm=fwhmptr->getvalue();
  const Parameter* gapstartptr= getparameter(10);
  const double gapstart=gapstartptr->getvalue();
   const Parameter* gapendptr= getparameter(11);
  const double gapend=gapendptr->getvalue();

  #ifdef COMPONENT_DEBUG
  std::cout<<"Calculating spin split edge\n";
  std::cout<<"E0="<<E0<<" Ek="<<Ek<<" thetasteps="<<thetasteps<<" thetamax="<<thetamax<<"strength="<<strength<<"\n";
  std::cout<<"Esplit="<<Esplit<<" ratio="<<ratio<<" wstrength="<<wstrength<<" fwhm="<<fwhm<<" gapstart="<<gapstart<<" gapend="<<gapend<<"\n";

  #endif


  //recalculate subparts if parameters have changed
    //update the internal parameters for these models


    if (E0ptr->changed()) (continuum1->getparameter(0))->setvalue(E0);
    if (Ekptr->changed()) (continuum1->getparameter(1))->setvalue(Ek);
    if (thetastepptr->changed()) (continuum1->getparameter(3))->setvalue(thetasteps);
    if (thetamaxptr->changed()) (continuum1->getparameter(4))->setvalue(thetamax);
    if (strengthptr->changed()||ratioptr->changed()) (continuum1->getparameter(5))->setvalue(strength/(1.0+ratio));



    if (E0ptr->changed()) (continuum2->getparameter(0))->setvalue(E0);
    if (Ekptr->changed()||Esplitptr->changed()) (continuum2->getparameter(1))->setvalue(Ek+Esplit);
    if (thetastepptr->changed()) (continuum2->getparameter(3))->setvalue(thetasteps);
    if (thetamaxptr->changed()) (continuum2->getparameter(4))->setvalue(thetamax);
    if (strengthptr->changed()||ratioptr->changed()) (continuum2->getparameter(5))->setvalue(strength*ratio/(1.0+ratio));




    continuum1->calculate();
    continuum2->calculate();


    if (Ekptr->changed()) (whiteline1->getparameter(0))->setvalue(Ek);
    if (fwhmptr->changed()) (whiteline1->getparameter(1))->setvalue(fwhm);
    if (wstrengthptr->changed()) (whiteline1->getparameter(2))->setvalue(wstrength);

    if (Ekptr->changed()||Esplitptr->changed()) (whiteline2->getparameter(0))->setvalue(Ek+Esplit);
    if (fwhmptr->changed()) (whiteline2->getparameter(1))->setvalue(fwhm);
    if (wstrengthptr->changed()||ratioptr->changed()) (whiteline2->getparameter(2))->setvalue(wstrength*ratio);
    whiteline1->calculate();
    whiteline2->calculate();
    //add up the different components
    for (size_t i=0;i<this->getnpoints();i++){
      double cts=0.0;
      const double en=this->getenergy(i);
      if ((Ek+Esplit+gapstart<=en)&&(Ek+Esplit+gapend>en)){
        //continuum2 gap
         cts=continuum1->getcounts(i)+whiteline1->getcounts(i)+whiteline2->getcounts(i);
      }
      else
      {
         if ((Ek+gapstart<=en)&&(Ek+gapend>en)){
           //continuum1 gap
           cts=continuum2->getcounts(i)+whiteline1->getcounts(i)+whiteline2->getcounts(i);
         }
         else{
          //no gap
           cts=continuum1->getcounts(i)+continuum2->getcounts(i)+whiteline1->getcounts(i)+whiteline2->getcounts(i);
         }
      }
      this->setcounts(i,cts);
    }
    this->setunchanged();
    return;
}

SpinsplitEdge* SpinsplitEdge::clone()const{
  return new SpinsplitEdge(*this);
}
SpinsplitEdge* SpinsplitEdge::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new SpinsplitEdge(n,estart,dispersion,parameterlistptr);
}
