/***************************************************************************
                          edgefile.cpp  -  description
A component which gets it spectrum from an existing spectrum, or loads it via the filename
The spectrum is automaticaly interpolated to the right energy dispersion of the model
The spectrum can be shifted, energy scaled, and amplitude scaled.
Preferable the edge file contains an EELS edge that has its onset at 0eV
Use this componet to load experimental edges or calculated edges for example from ab initio calculations
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

#include "src/components/fixedpattern.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>
#include "src/gui/getgraphptr.h"
#include <QWorkspace>



extern QWorkspace* getworkspaceptr();

FixedPattern::FixedPattern()
:Component()
{
  this->setname("FixedPattern");
  this->setdescription("y=fixed pattern spectrum loaded from file, can be shifted and scaled\nMake sure the edge file is calibrated to 0eV at the edge onset");
}

FixedPattern::FixedPattern(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Spectrum* edgeptr=0;
  if (parameterlistptr==0){
    //get a pointer to an edge file
    QWorkspace* myworkspace=getworkspaceptr();
    Getgraphptr* getptr=new Getgraphptr(myworkspace,"","Make sure an fixed pattern spectrum is frontmost\nMake sure the edge file is calibrated to 0eV at the edge onset");
    getptr->show();
    if ( getptr->exec() == QDialog::Accepted ) {
      edgeptr=getptr->getpointer();
    }
    delete getptr;
    if (edgeptr==0) throw Componenterr::unable_to_create();
    p4=new Parameter(edgeptr->getfilename(),0.0,0);//store the filename in an extra parameter

    //add the parameters
    p1= new Parameter("Ek",0.0,1);
    p1->interactivevalue("enter Ek, the edge position");
    p1->setchangeable(false);
    p2=new Parameter("Strength",1.0,1);
    p2->interactivevalue("enter Strength");
    p2->setchangeable(false);
    p3=new Parameter("Scale",1.0,0);
    p3->interactivevalue("enter Scale factor to stretch/shrink energy scale");
    p3->setchangeable(false);
  }
  else{
    //get the parameters
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
    p3=(*parameterlistptr)[2];
    p4=(*parameterlistptr)[3];
    //get the edge file via the filename
    loadmsa l;
    edgeptr = new Spectrum(l,p4->getname()); //the filename is stored in the name of p4
    if (edgeptr==0) throw Componenterr::unable_to_create();
  }
  p2->setlinear(true);
  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4); //the dummy parameter which contains the filename
  //give a name and description
  this->setname("FixedPattern");
  this->setdescription("y=spectrum loaded from file, can be shifted and scaled");
  //no convolution
  this->setcanconvolute(false);
  setshifter(false);

  //make a copy of the edge spectrum, so we don't lose it when somebody kills the original
  if (edgeptr!=0){
    //copy the spectrum via the copy assignment defined in Spectrum class
    edge.setnpoints(edgeptr->getnpoints());//resize edge so *edgeptr will fit in it
    edge=(*edgeptr);
  }
  else throw Componenterr::unable_to_create();
}

FixedPattern::~FixedPattern(){
  //nothing to do
}

void FixedPattern::calculate()
{
  //check if any of paramters is changed, if not : don't calculate
  const Parameter* p1= getparameter(0);
  const double Ek=p1->getvalue();
  const Parameter* p2= getparameter(1);
  const double strength=p2->getvalue();
  const Parameter* p3= getparameter(2);
  const double scale=p3->getvalue();
  //set p4 unchangeable, whatever the user does, never change the filename!
  Parameter* p4= getparameter(3);
  p4->setchangeable(false);

  if (p1->changed()||p2->changed()||p3->changed())
    {
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating edge file   \n Ek: " << Ek << " strength:" <<strength<< "scale"<< scale <<"\n";
    #endif
    for (unsigned int i=0;i<(this->getnpoints());i++){
      const double en=this->getenergy(i);
      double cts=0.0;
      //use linear interpolation to copy the edge file to a different position and scale in the components spectrum
      for (unsigned int j=0;j<edge.getnpoints();j++){
        //go through the edge until scaled edgeenergy + Ek >=en
        const double Eedge=(edge.getenergy(j)*scale)+Ek;
        if ((Eedge>en)&&(j>=1)){
          const double Eedgeprev=(edge.getenergy(j-1)*scale)+Ek;
          if (Eedgeprev>en) { //both are higher, we are outside of the region where we know the edge
            cts=0.0;
            break;
          }
          const double edgecounts=edge.getcounts(j);
          const double edgeprevcounts=edge.getcounts(j-1);
          const double rico= (edgecounts- edgeprevcounts)/(Eedge-Eedgeprev);
          const double delta= en-Eedgeprev;
          cts=strength*(edgeprevcounts+rico*delta);
          break;
        }
        if (Eedge==en){
          cts=strength*(edge.getcounts(j));
          break;
        }
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

FixedPattern* FixedPattern::clone()const{
  return new FixedPattern(*this);
}

FixedPattern* FixedPattern::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new FixedPattern(n,estart,dispersion,parameterlistptr);
}
