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
 * eelsmodel - components/edgefile.cpp
 **/

#include "src/components/edgefile.h"

#include <cmath>
#include <iostream>

#include <QWorkspace>

#include "src/core/parameter.h"

#include "src/gui/getgraphptr.h"

QWorkspace* getworkspaceptr();

EdgeFile::EdgeFile()
:Component(),tempspectrum()
{
  this->setname("EdgeFile");
  this->setdescription("y=spectrum loaded from file, can be shifted and scaled\nMake sure the edge file is calibrated to 0eV at the edge onset");
}

EdgeFile::EdgeFile(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion)
{
  //add the required parameters
  Spectrum* edgeptr=0;
  if (parameterlistptr==0){
    //get a pointer to an edge file
    QWorkspace* myworkspace=getworkspaceptr();
    Getgraphptr* getptr=new Getgraphptr(myworkspace,"","Make sure an edge spectrum is frontmost\nMake sure the edge file is calibrated to 0eV at the edge onset");
    getptr->show();
    if ( getptr->exec() == QDialog::Accepted ) {
      edgeptr=getptr->getpointer();
    }
    delete getptr;
    if (edgeptr==0) throw Componenterr::unable_to_create();
    Parameter* p4=new Parameter(edgeptr->getfilename(),0.0,0);//store the filename in an extra parameter

    //add the parameters
    Parameter* p1=new Parameter("Ek",estart+double(n/2)*dispersion,1);
    p1->interactivevalue("enter Ek, the edge position");
    Parameter* p2=new Parameter("Strength",1.0,1);
    p2->interactivevalue("enter Strength");
    p2->setlinear(true);
    Parameter* p3=new Parameter("Scale",1.0,0);
    p3->interactivevalue("enter Scale factor to stretch/shrink energy scale");
    this->addparameter(p1);
    this->addparameter(p2);
    this->addparameter(p3);
    this->addparameter(p4); //the dummy parameter which contains the filename
  }
  else{
    //get the parameters
    Parameter* p1=(*parameterlistptr)[0];
    Parameter* p2=(*parameterlistptr)[1];
    p2->setlinear(true);
    Parameter* p3=(*parameterlistptr)[2];
    Parameter* p4=(*parameterlistptr)[3];
    //get the edge file via the filename
    loadmsa l;
    edgeptr = new Spectrum(l,p4->getname()); //the filename is stored in the name of p4
    if (edgeptr==0) throw Componenterr::unable_to_create();
    p4->setname(edgeptr->getfilename()); //update p4 in case the filename was not found and the user directed us to a new filename and/or path
    this->addparameter(p1);
    this->addparameter(p2);
    this->addparameter(p3);
    this->addparameter(p4); //the dummy parameter which contains the filename
  }

  //give a name and description
  this->setname("EdgeFile");
  this->setdescription("y=spectrum loaded from file, can be shifted and scaled");
  this->setcanconvolute(true);
  setshifter(false);


  //make a copy of the edge spectrum, so we don't lose it when somebody kills the original
  if (edgeptr!=0){
    //copy the spectrum via the copy assignment defined in Spectrum class
    edge.setnpoints(edgeptr->getnpoints());//resize edge so *edgeptr will fit in it
    edge=(*edgeptr);
  }
  else throw Componenterr::unable_to_create();
}

EdgeFile::~EdgeFile(){
  //nothing to do
}

void EdgeFile::calculate()
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

  if (p1->changed()||p3->changed())
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
          cts=(edgeprevcounts+rico*delta);
          break;
        }
        if (Eedge==en){
          cts=(edge.getcounts(j));
          break;
        }
      }     
      //and store it in a tempspectrum in case only the strength changes, save a lot of time
      tempspectrum.setdatapoint(i,en,cts,0.0);//store the shape in the temp spectrum
      this->setdatapoint(i,en,cts*strength,0.0);//then, multiply by the strength in the component spectrum         
    }
  }
  else{
      if(p2->changed()){
        //if only the strength has changed take the spectrum from the tempspectrum and multiply with strength
        //this is a lot faster and important for the fitting routines
        for (unsigned int i=0;i<(this->getnpoints());i++){
          const double E=this->getenergy(i);
          this->setdatapoint(i,E,tempspectrum.getcounts(i)*strength,0.0);
          }

        #ifdef COMPONENT_DEBUG
        std::cout <<"parameters have not changed, only strength has changed\n";
        #endif
        }
      #ifdef COMPONENT_DEBUG
      std::cout <<"parameters have not changed, do nothing\n";
      #endif
      }
  //set parameters as unchanged since last time we calculated
  this->setunchanged();
}

EdgeFile* EdgeFile::clone()const{
  return new EdgeFile(*this);
}

EdgeFile* EdgeFile::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new EdgeFile(n,estart,dispersion,parameterlistptr);
}
