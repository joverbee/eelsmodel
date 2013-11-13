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
 * eelsmodel - core/component.cpp
 **/

#include "component.h"

#include <sstream>
#include <vector>

#include "src/gui/eelsmodel.h"
#include "src/gui/equalizer.h"
#include "src/gui/graph.h"
#include "src/gui/saysomething.h"

class QWorkspace;

EELSModel* geteelsmodelptr();
QWorkspace* getworkspaceptr();

Component::Component()
  :Spectrum(1,0,1.0),parametervector(),hasgradient(),gradient(1,0,1.0)
{
    myequalizer=0;
  //create a dummy version
   this->setconvolutor(false);
  this->setshifter(false);
  this->setcanconvolute(true);
  this->set_ismultiplier(false);
  this->setvisible(false);
  multiplierptr=0;
  hasmultiplier=false;
  gradient.setpppc(1.0);
  setdescription("unitialized component");
  name = "unitialized component";
}

Component::Component(int n ,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
  :Spectrum(n,estart,dispersion),parametervector(),hasgradient(),gradient(n,estart,dispersion)
{
    myequalizer=0;
  //this->sethasgradient(false);
  this->setconvolutor(false);
  this->setshifter(false);
  this->setcanconvolute(true);
  this->set_ismultiplier(false);
  this->setvisible(false);
  multiplierptr=0;
  hasmultiplier=false;
  gradient.setpppc(1.0);
  setdescription("unitialized component");
  name = "unitialized component";
 }

Component::~Component(){


  //gradient is  automagically deleted because we own it
  if (myequalizer!=0){
      delete(myequalizer);
    }
  //delete all parameters, we own them since we got them via addparameter
  for (size_t i=0;i<parametervector.size();i++){
    delete (parametervector[i]);
  }
}


//void calculate(); //each component has to define how to do this: pure virtual class, you can not create a component
void Component::calculate(){}

void Component::setdescription(std::string s){
  description=s;
}

void Component::addparameter(Parameter* p){
  try{
    p->setchanged(); //make sure to treat it as changed first time it occurs
    parametervector.push_back(p);
    hasgradient.push_back(false); //add a false entry in the gradient vector
  }
  catch(...){//probably unable to allocate memory
      #ifdef COMPONENT_DEBUG
      std::cout <<"cought an exception when trying to add a parameter\n";
      #endif
      throw Component::Componenterr::unable_to_create();
  }
}

void Component::setcanconvolute(bool b){
  canconvolute=b;
}

void Component::sethasgradient(size_t j,bool b){
  if (j<hasgradient.size()){
    hasgradient[j]=b;
  }
}

void Component::setunchanged(){
  //set all parameters in the component to unchanged
  for (unsigned int i=0;i<parametervector.size();i++)
  {
    parametervector[i]->setunchanged();
  }
}

void Component::setchanged(){
  //set all parameters in the component to changed, this forces a recalc if calculate is called
  for (unsigned int i=0;i<parametervector.size();i++)
  {
    parametervector[i]->forcechanged();
  }
}

void Component::seteshift(double e){
	//in case of an eshift in a component, also set to changed
	//this forces a recalc when calculate is called
	Spectrum::seteshift(e);
	this->setchanged();
}

//inspectors
std::string Component::getname()const{
  return name;
}

std::string Component::getdisplayname()const{
  return typname;
}

std::string Component::getdescription()const{
  return description;
}

Parameter* Component::getparameter(unsigned int index) {
  if (indexOK(index)) {
    return (parametervector[index]);
  }
  else {
    throw Component::Componenterr::bad_index();
    return 0;
  }
}

const Parameter* Component::getparameter(unsigned int index) const{
  if (indexOK(index)) {
    return (parametervector[index]);
  }
  else {
    throw Component::Componenterr::bad_index();
    return 0;
  }
}



bool Component::indexOK(unsigned int index)const{
  return  (index<=parametervector.size());
}


Component* Component::getmultiplierptr()
{
    if (this->hasmultiplier==false){
        return 0;
    }
    //test the component
  try{
    const bool dummy=multiplierptr->get_ismultiplier();
    (void) dummy;
  }
  catch(...){
    #ifdef COMPONENT_DEBUG
      std::cout <<"cought an exception when trying to use the multiplier comp\n";
    #endif
    hasmultiplier=false;
    multiplierptr=0;
    return 0;
  }
  return multiplierptr;
}
void Component::setmultiplierptr(Component* comp)
{
  //test the component
  bool dummy=false;
  try{
    dummy=comp->get_ismultiplier();
  }
  catch(...){
    hasmultiplier=false;
    multiplierptr=0;
    #ifdef COMPONENT_DEBUG
      std::cout <<"catched an exception when trying to use the multiplier comp\n";
    #endif
  }
  if (dummy==true) {
    #ifdef COMPONENT_DEBUG
    std::cout <<"multiplier:"<<comp->getname()<<" pointer accepted\n";
    #endif
    multiplierptr=comp;
    hasmultiplier=true;
  }
  else{
    #ifdef COMPONENT_DEBUG
      std::cout <<"multiplier:"<<comp->getname()<<" pointer not accepted dummy=0\n";
    #endif
    multiplierptr=0;
    hasmultiplier=false;
  }
}
void Component::set_ismultiplier(bool b)
{
  multiplier=b;
  //a multiplier can not have a multiplier itself
  hasmultiplier=false;
  multiplierptr=0;
  #ifdef COMPONENT_DEBUG
      std::cout <<"set multiplier to:"<<multiplier<<" \n";
  #endif
}
void Component::releasemultiplier(){
  //tell the component that we no longer have a multiplier
  hasmultiplier=false;
  multiplierptr=0;
 #ifdef COMPONENT_DEBUG
      std::cout <<"released the multiplier\n";
  #endif
}
void Component::setvisible(bool b){
  visible=b;
  show(); //show it if it was set to true
}
void Component::show(){
  if (isvisible()){
    //initiate or update a drawing of this in a separate window

    this->display(getworkspaceptr());
  }
}
bool Component::hasparameter(Parameter* p){
    //return true if p is one of our paramters
     for (size_t i=0;i<parametervector.size();i++)
  {
    if ((parametervector[i])==p) return true;
  }
return false;
}

void Component::showequalizer(){
    //shows an equalizer window to graphically control the parameter values
    if (myequalizer!=0){
        //delete old one if already exists
        delete(myequalizer);
        }
    std::ostringstream namestream;
    if (this->getdisplayname()!=""){
        if ((namestream << "Equalizer:"<< this->getdisplayname())){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     std::string caption=namestream.str();
	     myequalizer=new Equalizer(getworkspaceptr(),caption.c_str(),this);
        }
    }
        else{
         if ((namestream << "Equalizer:"<< this->getname())){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     std::string caption=namestream.str();
	     myequalizer=new Equalizer(getworkspaceptr(),caption.c_str(),this);
        }
    }
}
void Component::pullparameter(){
    //remove last parameter from the parameter list
  try{
    delete (parametervector.back()); //delete the param
    parametervector.pop_back(); //and remove the pointer from the vector
    hasgradient.pop_back();
  }
  catch(...){//probably unable to allocate memory
      #ifdef COMPONENT_DEBUG
      std::cout <<"catched an exception when trying to pull a parameter\n";
      #endif
      throw Component::Componenterr::unable_to_create();
  }
}
