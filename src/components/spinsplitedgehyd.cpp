/***************************************************************************
                         spinsplitedgehyd.h  -  description
A spin splitted edge with white lines, can be a L2,3 or M4,5 edge
build from two hydrogenic L cross sections and two lorentzians
07-01-2004: added adjustable energy gap to better simulate edges in insulators, the 3d levels remain at same position
  but jumps to the continuum need to surpass the energy gap.
08-01-2004: changed lorentzian to gaussians because they give generally a better fit
08-01-2004 added an extra band gap to better simulate perovskites, they seem to have a bandgap in the continuum region
18-10-2004 corrected an error in the continuum part: the total strength was off by a factor (1+ratio)
11-11-2009 added t2g-eg splitting

this edge models a spin split core state p or d with jumps to a partially occupied d or f state and
jumps to the continuum spectrum

e.g an L2,3 edge is created by
jump from 2p3/2 to 3d requiring energy Ek
jump from 2p1/2 to 3d requiring energy Ek+Esplit
jump from 2p3/2 to continuum states requiring Ek
jump from 2p1/2 to continuum states requiring Ek+Esplit
a gap is created in the continuum bands from Ek+gapstart til EK+ gapend for the first part
and Ek+Esplit+gapstart till Ek+Esplit+gapend for the second part

the ratio in occupation of 3/2 and 1/2 is adjustable with the ratio parameter
nominally this should be 0.5 for L2,3 and 4/6 for M4,5, in practice these ratios can deviate (anomalous white line ratios)
because of many body effects
the same ratio is taken for the jumps to the continuum




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


#include "src/components/spinsplitedgehyd.h"
#include "src/core/parameter.h"
#include "src/gui/getfilename.h"
#include <cmath>
#include <iostream>
#include <string>
#include <fstream>
#include "src/core/curvematrix.h"
#include "src/gui/imagedisplay.h"
#include "src/components/ledge.h"
#include "src/components/gaussian.h"
#include "src/components/lorentz.h"
#define GAUSS


SpinsplitEdgeHyd::SpinsplitEdgeHyd() //create a dummy version
:Component(),tempspectrum(){
  this->setname("Hydrogenic Spin split edge section");
  this->setdescription("An edge consisting of two hydrogenic cross sections and two lorentzian white lines to simulate L23 edges");
  //init the pointers
  continuum1=0;
  continuum2=0;
  whiteline1=0;
  whiteline2=0;
  whiteline1eg=0;
  whiteline2eg=0;
}

SpinsplitEdgeHyd::SpinsplitEdgeHyd(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion){
  //init the pointers
  continuum1=0;
  continuum2=0;
  whiteline1=0;
  whiteline2=0;
  whiteline1eg=0;
  whiteline2eg=0;
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
  //new params
  Parameter* p12; //t2gegsplit
  Parameter* p13; //t2gegratio
  
  if (parameterlistptr==0){
    p1=new Parameter("E0",300e3,1);
    p1->interactivevalue("enter primary energy [eV]");
    p1->setchangeable(false);  //the fitter should not change this in normal operation

    p2=new Parameter("Ek",estart,1);
    p2->interactivevalue("enter edge energy [eV]");

    p3=new Parameter("Z",25,1);
    p3->interactivevalue("enter atomic number");
    p3->setchangeable(false); //the fitter should not change this in normal operation
    
    p5=new Parameter("thetamax",2.0e-3,1);
    p5->interactivevalue("enter collection half angle theta [rad]");
    p5->setchangeable(false); //the fitter should not change this in normal operation

    p11=new Parameter("convergence angle",0.0e-3,1);
    p11->interactivevalue("enter convergence half angle alpha [rad]");
    p11->setchangeable(false); //the fitter should not change this in normal operation

    p4=new Parameter("theta steps",100.0,1);
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
    
    p12=new Parameter("t2g/eg split",1.0e1,1);
    p12->interactivevalue("enter t2g/eg splitting energy");
    
    p13=new Parameter("t2g/eg ratio",1.0e1,1);
    p13->interactivevalue("enter t2g/eg ratio");
        
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
    p11=(*parameterlistptr)[10];
    if (parameterlistptr->size()>11){
    	 p12=(*parameterlistptr)[11];
    	 p13=(*parameterlistptr)[12];
    }
    else{
    	//enter zero t2g eg splitting
    	p12=new Parameter("t2g/eg split",0,1);    	       	   
    	p13=new Parameter("t2g/eg ratio",1.0e1,1);    	        	
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
    std::cout <<"p13 "<<p13->getname()<<" \n";
    #endif
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
  this->addparameter(p13);

  //create the components
  const double E1=p2->getvalue();
  const double E2=p2->getvalue()+p7->getvalue();
  const double strength=p7->getvalue();

  const double wstrength1=p9->getvalue();
  const double wstrength2=wstrength1*p8->getvalue();
  const double fwhm=p10->getvalue();

  const double ratio=p8->getvalue();
  const double t2gegratio=p13->getvalue();
  const double t2gegsplit=p12->getvalue();
  
  //create first continuum part
  std::vector<Parameter*> cont1par;
  const double S1=strength/(1.0+ratio);
  Parameter* pS1=new Parameter("Strength1",S1,1);
  Parameter* ph1=new Parameter(*p1);
  Parameter* ph2=new Parameter(*p2);
  Parameter* ph3=new Parameter(*p3);
  Parameter* ph4=new Parameter(*p4);
  Parameter* ph5=new Parameter(*p5);
  Parameter* ph11=new Parameter(*p11);

  cont1par.push_back(ph1);
  cont1par.push_back(ph2);
  cont1par.push_back(ph3);
  ph5->setchangeable(true); //make sure "this" can still change the value stored in p5
  ph4->setchangeable(true); //make sure "this" can still change the value stored in p5
  //this does not affect the user accessible parameters of this because they are already added to the paramlist
  cont1par.push_back(ph4);
  cont1par.push_back(ph5);
  cont1par.push_back(pS1);
  cont1par.push_back(ph11);

  //cont1par.push_back(pS1);
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

  continuum1=new Ledge(n,estart,dispersion,&cont1par); //create a hydrogenic L edge

  #ifdef COMPONENT_DEBUG
  std::cout<<"continuum1 ptr="<<continuum1<<"\n";
  #endif
  //create second continuum part,same as continuum1 but add spin split to Ek
  const double S2=strength*ratio/(1.0+ratio);
  Parameter* pS2=new Parameter("Strength2",S2,1);

  Parameter* pi1=new Parameter(*p1);
  Parameter* pi2=new Parameter(*p2);
  pi2->setvalue(p2->getvalue()+p7->getvalue());
  Parameter* pi3=new Parameter(*p3);
  Parameter* pi4=new Parameter(*p4);
  Parameter* pi5=new Parameter(*p5);
  Parameter* pi11=new Parameter(*p11);

  std::vector<Parameter*> cont2par;
  cont2par.push_back(pi1);
  cont2par.push_back(pi2);
  cont2par.push_back(pi3);
  cont2par.push_back(pi4);
  cont2par.push_back(pi5);
  cont2par.push_back(pS2);
  cont2par.push_back(pi11);

  //cont2par.push_back(pS2);
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

  continuum2=new Ledge(n,estart,dispersion,&cont2par);

  #ifdef COMPONENT_DEBUG
  std::cout<<"continuum2 ptr="<<continuum2<<"\n";
  #endif
  //create first white line
#ifdef GAUSS
  whiteline1=new Gaussian(n,estart,dispersion,E1-t2gegsplit/2.0,fwhm,1.0);
#else
  whiteline1=new Lorentz(n,estart,dispersion,E1-t2gegsplit/2.0,fwhm,1.0);
#endif
  
  
  #ifdef COMPONENT_DEBUG
  std::cout<<"whiteline1 ptr="<<whiteline1<<"\n";
  #endif
    
  //create second white line
#ifdef GAUSS
  whiteline2=new Gaussian(n,estart,dispersion,E2-t2gegsplit/2.0,fwhm,1.0);
#else
  whiteline2=new Lorentz(n,estart,dispersion,E2-t2gegsplit/2.0,fwhm,1.0);
#endif
  
  #ifdef COMPONENT_DEBUG
  std::cout<<"whiteline2 ptr="<<whiteline2<<"\n";
  #endif
  
  
 //and two eg white lines 
#ifdef GAUSS
  whiteline1eg=new Gaussian(n,estart,dispersion,E1+t2gegsplit/2.0,fwhm,1.0);
#else
 whiteline1eg=new Lorentz(n,estart,dispersion,E1+t2gegsplit/2.0,fwhm,1.0);
#endif
 
   #ifdef COMPONENT_DEBUG
   std::cout<<"whiteline1eg ptr="<<whiteline1eg<<"\n";
   #endif
   
#ifdef GAUSS
   whiteline2eg=new Gaussian(n,estart,dispersion,E2+t2gegsplit/2.0,fwhm,1.0);
#else
   whiteline2eg=new Lorentz(n,estart,dispersion,E2+t2gegsplit/2.0,fwhm,1.0);
#endif
   
   #ifdef COMPONENT_DEBUG
   std::cout<<"whiteline2eg ptr="<<whiteline2eg<<"\n";
   #endif
      
      
  //give a name and description
  this->setname("Hydrogenic Spin split edge section");
  this->setdescription("An edge consisting of two hydrogenic cross sections and two lorentzian white lines to model an L23 edge");
   this->setcanconvolute(true);
   this->setshifter(false);

  if (continuum1==0||continuum2==0||whiteline1==0||whiteline2==0||whiteline1eg==0||whiteline2eg==0) throw Componenterr::unable_to_create();
}


SpinsplitEdgeHyd::~SpinsplitEdgeHyd(){
  //clean up the components which we own
  if (continuum1!=0)    delete(continuum1);
  if (continuum2!=0)    delete(continuum2);
  if (whiteline1!=0)    delete(whiteline1);
  if (whiteline2!=0)    delete(whiteline2);
  if (whiteline1eg!=0)    delete(whiteline1eg);
  if (whiteline2eg!=0)    delete(whiteline2eg);
}

void SpinsplitEdgeHyd::calculate(){
  //get the parameters
  const Parameter* E0ptr= getparameter(0);
  const double E0=E0ptr->getvalue();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  const Parameter* Zptr= getparameter(2);
  const double Z=Zptr->getvalue();
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
  const Parameter* alphaptr= getparameter(10);
  const double alpha=alphaptr->getvalue();
  
  const Parameter* t2gegsplitptr=getparameter(11);
  const double t2gegsplit=t2gegsplitptr->getvalue();
  const Parameter* t2gegratioptr=getparameter(12);
  const double t2gegratio=t2gegratioptr->getvalue();
    
    
  
  #ifdef COMPONENT_DEBUG
  std::cout<<"Calculating spin split edge\n";
  std::cout<<"E0="<<E0<<" Ek="<<Ek<<" thetasteps="<<thetasteps<<" thetamax="<<thetamax<<"strength="<<strength<<"\n";
  std::cout<<"Esplit="<<Esplit<<" ratio="<<ratio<<" wstrength="<<wstrength<<" fwhm="<<fwhm<<" alpha="<<alpha<<"\n";

  #endif


  //recalculate subparts if parameters have changed
    //update the internal parameters for these models

    if (Ekptr->changed()) (continuum1->getparameter(1))->setvalue(Ek);
    if (thetastepptr->changed()) (continuum1->getparameter(3))->setvalue(thetasteps);
    if (thetamaxptr->changed()) (continuum1->getparameter(4))->setvalue(thetamax);
    if (strengthptr->changed()||ratioptr->changed()) (continuum1->getparameter(5))->setvalue(strength/(1.0+ratio));

    if (Zptr->changed()||alphaptr->changed()||E0ptr->changed()){
     (continuum1->getparameter(0))->setvalue(E0);
     (continuum2->getparameter(0))->setvalue(E0);
     (continuum1->getparameter(2))->setvalue(Z);
     (continuum2->getparameter(2))->setvalue(Z);
     (continuum1->getparameter(6))->setvalue(alpha);
     (continuum2->getparameter(6))->setvalue(alpha);
    }

    if (Ekptr->changed()||Esplitptr->changed()) (continuum2->getparameter(1))->setvalue(Ek+Esplit);
    if (thetastepptr->changed()) (continuum2->getparameter(3))->setvalue(thetasteps);
    if (thetamaxptr->changed()) (continuum2->getparameter(4))->setvalue(thetamax);
    if (strengthptr->changed()||ratioptr->changed()) (continuum2->getparameter(5))->setvalue(strength*ratio/(1.0+ratio));




    continuum1->calculate();
    continuum2->calculate();

    //t2g and eg white lines
    if (Ekptr->changed()||t2gegsplitptr->changed()){
    	(whiteline1->getparameter(0))->setvalue(Ek-t2gegsplit/2.0);
    	(whiteline1eg->getparameter(0))->setvalue(Ek+t2gegsplit/2.0);    	
    }
    if (fwhmptr->changed()){
    	(whiteline1->getparameter(1))->setvalue(fwhm);
    	(whiteline1eg->getparameter(1))->setvalue(fwhm);
    }
    //if (wstrengthptr->changed()||t2gegratioptr->changed()||strengthptr->changed()){
    //	(whiteline1->getparameter(2))->setvalue(wstrength*ratio*t2gegratio*strength);
    //	(whiteline1eg->getparameter(2))->setvalue(wstrength*ratio*(1.0-t2gegratio)*strength);    	
    //}

    if (Ekptr->changed()||Esplitptr->changed()||t2gegsplitptr->changed()){
    	(whiteline2->getparameter(0))->setvalue(Ek+Esplit-t2gegsplit/2.0);
    	(whiteline2eg->getparameter(0))->setvalue(Ek+Esplit+t2gegsplit/2.0);
    }
    if (fwhmptr->changed()){
    	(whiteline2->getparameter(1))->setvalue(fwhm);
    	(whiteline2eg->getparameter(1))->setvalue(fwhm);
    }
    //if (wstrengthptr->changed()||ratioptr->changed()||t2gegratioptr->changed()||strengthptr->changed()){
    //	(whiteline2->getparameter(2))->setvalue(wstrength*(1.0-ratio)*t2gegratio*strength);
    //	(whiteline2eg->getparameter(2))->setvalue(wstrength*(1.0-ratio)*strength*(1.0-t2gegratio));
    	
    //}
    whiteline1->calculate();
    whiteline2->calculate();    
    whiteline1eg->calculate();
    whiteline2eg->calculate();
    
    whiteline1->normalize();
    whiteline2->normalize();
    whiteline1eg->normalize();
    whiteline2eg->normalize();
    
    
    
    //add up the different components
    const double w1strengtheg=wstrength*strength/((1.0+1.0/ratio)*(1.0+t2gegratio));
    const double w1strength=w1strengtheg*t2gegratio;    
    const double w2strengtheg=wstrength*strength/((1.0+ratio)*(1.0+t2gegratio));
    const double w2strength=w2strengtheg*t2gegratio;
                   
    
    for (size_t i=0;i<this->getnpoints();i++){
      const double cts=continuum1->getcounts(i)+continuum2->getcounts(i)+w1strength*whiteline1->getcounts(i)+w2strength*whiteline2->getcounts(i)+w1strengtheg*whiteline1eg->getcounts(i)+w2strengtheg*whiteline2eg->getcounts(i);
      this->setcounts(i,cts);
    }
    this->setunchanged();
    return;
}

SpinsplitEdgeHyd* SpinsplitEdgeHyd::clone()const{
  return new SpinsplitEdgeHyd(*this);
}
SpinsplitEdgeHyd* SpinsplitEdgeHyd::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new SpinsplitEdgeHyd(n,estart,dispersion,parameterlistptr);
}
