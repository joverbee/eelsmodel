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
 * eelsmodel - core/parameter.cpp
 **/

#include "src/core/parameter.h"

//#define PARAMETER_DEBUG

#include <string>
#include <iostream>
#include <sstream>
#include <limits>

#include <QWorkspace>

#include "src/core/monitor.h"

#include "src/gui/saysomething.h"
#include "src/gui/interactive_parameter.h"

QWorkspace* getworkspaceptr();

Parameter::Parameter(){
  setdefaults();
  setvalue(0.0);
}

Parameter::Parameter(double val){
  setdefaults();
  setvalue(val);
}

Parameter::Parameter(std::string namestr,double val){
    setdefaults();
  setvalue(val);
  setname(namestr);
}

Parameter::Parameter(std::string namestr,double val,bool changeallowed)
{
#ifdef PARAMETER_DEBUG
  std::cout <<"constructing parameter\n";
  std::cout <<"namestr:"<<namestr<<"\n";
  std::cout <<"val:"<<val<<"\n";
  std::cout <<"changeallowed:"<<changeallowed<<"\n";
#endif
  setdefaults();
  setname(namestr);
  setvalue(val);
  setchangeable(changeallowed); //now put users changeable
}

Parameter::~Parameter(){
	
  //release the coupling if you are still attached to another component
  deletecoupling();
  
  //kill the monitor if you have a monitor or if you are watched by a monitor
  killmonitor();
  #ifdef PARAMETER_DEBUG
  std::cout << "Parameter :"<<this->getname()<<": killed\n";
  #endif
}


Parameter::Parameter(const Parameter& P)
//public copy constructor
//important to get the monitor pointers etc right, otherwise a double killing of a pointer occurs and
//causes strange crashes
//monitors and coupling are NOT copied!
{
    this->setdefaults();
    value=P.value;
    sigma=P.sigma;
    isschanged=true;
    changeable=P.changeable;
    bound=P.bound;

    lowerbound=P.lowerbound;
    upperbound=P.upperbound;
    name=P.name;
    linear=P.linear; //is this parameter a linear parameter in a model?
    warn=P.warn;

    //the coupling
    //coupling is not copied
     if (coupled) this->deletecoupling();
    coupled=false;
    coupled_parameter=0; //the pointer
    couple_fraction=1.0;

    //the monitoring
    //monitor is not copied
    killmonitor();
    master=false;
    monitored=false;
    watched=false;
    monitorptr=0; //a pointer to a monitor that watches this parameter
    watchptr=0; //a pointer to a parameter that is watching you via a monitor

}

Parameter& Parameter::operator=(const Parameter& P)
//public assignment operator
{
    this->setdefaults();
    if (this!=&P){
    value=P.value;
    sigma=P.sigma;
    isschanged=true;
    changeable=P.changeable;
    bound=P.bound;

    lowerbound=P.lowerbound;
    upperbound=P.upperbound;
    name=P.name;
    linear=P.linear; //is this parameter a linear parameter in a model?
    warn=P.warn;

    //the coupling
    //coupling is not copied
    if (coupled) this->deletecoupling();
    coupled=false;
    coupled_parameter=0; //the pointer
    couple_fraction=1.0;

    //the monitoring
    //monitor is not copied
    this->killmonitor();
    master=false;
    monitored=false;
    watched=false;
    monitorptr=0; //a pointer to a monitor that watches this parameter
    watchptr=0; //a pointer to a parameter that is watching you via a monitor
    }
return *this;
}



void Parameter::setdefaults(){
  setdisplayed(false);
  setchangeable(true);
  coupled=false;
  master=false;
  bound=false;
  monitored=false;
  watched=false;
  linear=false;
  watchptr=0;
  coupled_parameter=0;
  monitorptr=0;
  setname("");
  sigma=0.0;
  setboundaries(-(std::numeric_limits<double>::max)(),(std::numeric_limits<double>::max)());
  warn=false;
}

void Parameter::setvalue(double x)
    {
      //only change when changeable and within boundaries, otherwise do nothing...
      if ((coupled)&&(isslave())) return; //this parameter cant'be changed
       if ((coupled)&&(ismaster())){
          //mark the slave as changed
          coupled_parameter->setchanged();
          //important!!!
          }
      if ((changeable)&&(boundaryOK(x,getupperbound(),getlowerbound())))
	{
	  value=x;
	  isschanged=true;
	}
	if (!boundaryOK(x,getupperbound(),getlowerbound())){
            //hitting the boundary
            if (warn){
                std::ostringstream s;
                s << "Hitting the boundary on parameter ' "<< this->getname() <<"' \n upperboundary="<<getupperbound()<<"  lowerbound="<<getlowerbound()<<"  requested value="<<x<<"\n either increase the boundary or try to find out why the fit is diverging ";
                std::string msg=s.str();
                Saysomething mysay(0,"Warning",msg.c_str(),false);
            }
    }
    }

void Parameter::setsigma(double x){
  sigma=x;
  }

void Parameter::interactivevalue(std::string description){
  //do an interactive setup of the parameter
  QWorkspace* myworkspace=getworkspaceptr();
  Interactive_Parameter* myinteractive=new Interactive_Parameter(myworkspace,"Parameter_init",this,description);
  myinteractive->show();
  if ( myinteractive->exec() == QDialog::Accepted ) {

    //get the info
    bound=false; //don't bind to circumvent a problem when the newvalue does not fit in the old bounds, we will put new bounds
    //anyway and the interactive parameter window already checks the bounds
    setchangeable(true); //if user entered something, don`t delete it
    //because it was locked, unlock temporarily
    setvalue(myinteractive->getvalue()); //update the value
    setchangeable(myinteractive->getcanchange()); //update the lock: do it after the editing of the value otherwise the value is not
    //updated if the user has chosen that the parameter can not be changed. This apperantly is confusing for most students on the QEM2005 school
    if (!iscoupledslave()){
    	setname(myinteractive->getname());//update the name, but only if not coupled slave
    }
    
    if (myinteractive->getbound()) setboundaries(myinteractive->getlbound(),myinteractive->getubound()); //update the boundaries if bound
    }
  	this->setdisplayed(myinteractive->isdisplayed());  	
  delete myinteractive;
  std::cout<<"the parameter "<<name<<" value is: "<<value<<"\n";
}

void Parameter::setunchanged(){
  isschanged=false;
}
void  Parameter::setdisplayed(bool b){
	displayed=b;	
}
bool Parameter::isdisplayed()const{
	return displayed;
}


void Parameter::setchanged(){
//this is a warning received by a slave of coupling
//the master warns him that his parameter is changed
if ((iscoupled())&&(isslave())){
  isschanged=true;//even if a slave is unchangeable...
  }
}

void Parameter::forcechanged(){
  isschanged=true;//force a parameter to be changed, this can be used to force the calculation to treat it as if something did change
}


double Parameter::getvalue()const {
  if ((iscoupled())&&(isslave())){
    //get the value of the coupled parameter if the master parameter still exists
    double coupledval;
    try{
      coupledval=coupled_parameter->getvalue();
    }
    catch(...){
      //something went wrong, return our own value      
      return value;
    }
    return coupledval*couple_fraction;
  }
  //in normal case, just return the value
  else return value;
}


Parameter* Parameter::getmaster() const{
//get a pointer to the master if this parameter is coupled as slave, else return 0
if (this->iscoupledslave()){
  return coupled_parameter;
  }
else{
  return 0;
  }
}

double Parameter::getsigma() const {
  return sigma;
}

bool Parameter::boundaryOK(double x,double up,double low) const {
    const bool upOK=x<=up;
    const bool lowOK=x>=low;
    const bool result=((!bound)||(upOK&&lowOK));
  return result;
}

bool Parameter::setboundaries(double x,double y)
{
//check if value fits in range, if not, don't set the boundaries
//return false if bounds are not set
bound=true;
  if (boundaryOK(value,y,x))
    {
      lowerbound=x;
      upperbound=y;
      return true;
    }
  if (boundaryOK(value,x,y))
    {
      lowerbound=y;
      upperbound=x;
      return true;
    }
bound=false;
return bound;
}


double Parameter::getupperbound() const {return upperbound;}
double Parameter::getlowerbound() const {return lowerbound;}
void  Parameter::setupperbound(double u){upperbound=u;}
void Parameter::setlowerbound(double l){lowerbound=l;}
void Parameter::setchangeable(bool b){changeable=b;}


void Parameter::couple(Parameter* p,double fraction){
  //we couple to a master, so we become slaves
  std::cout<<"coupling parameter "<<this->getname()<<" with : "<<p->getname()<<"\n";
  coupled=true;
  master=false;
  coupled_parameter=p; //pointer to the master
  couple_fraction=fraction;
  //test the master parameter to see if its a valid one
  try{
    double val=p->getvalue();
    (void) val;
  }
  catch(...){
    //something went wrong
    coupled=false;    
    coupled_parameter=0;
    return;
  }
  //check if the proposed parameter is not yourself
  //otherwise recursion! and crash
  if (this==p){
    //something went wrong
    std::cout<<"recursive coupling detected\n";
    coupled=false;
    coupled_parameter=0;
    return;
  }
  setchangeable(false); //we no longer need to be changeable by the user since from now on the master determines our value
  //now tell the master that we are here
  coupled_parameter->setmaster(this); 
}

void Parameter::setmaster(Parameter* slavepar){	
	if (slavepar!=0){	
		master=true;
		coupled=true;		
		coupled_parameter=slavepar;
	}
}

void Parameter::deletecoupling(){
	//get rid of the coupling
	if (!coupled) return; //nothing to do if not coupled
			
	try{
		//tell the master or the slave to forget the coupling
		if (coupled_parameter!=0) coupled_parameter->endcoupling();
	}
	catch(...){
		  //didn't work		  
	}
	//and forget about the coupling yourself	    
	endcoupling();

}

void Parameter::endcoupling(){
	//forget about your master or your slave
	//you're on your own now being a slave by default
	master=false;
	coupled=false;
	coupled_parameter=0;
}


void Parameter::setbound(bool b){
  if (b&&(boundaryOK(value,upperbound,lowerbound))) bound=true;
  else bound=false;
}

bool Parameter::changed() const {
  return isschanged;
}
bool Parameter::ischangeable() const{return changeable;}
bool Parameter::isbound() const{return bound;}
bool Parameter::iscoupled() const{return coupled;}
void Parameter::setname(std::string namestr){name=namestr;}
std::string Parameter::getname() const{
  if (iscoupledslave()){
    std::string totalname;
    totalname=name;
    totalname+= ":coupled to ";
    totalname+= coupled_parameter->getname();
    return totalname;
    }
  else return name;
}
std::string Parameter::getpurename() const{
  return name;
}

void Parameter::setwatched(Parameter* watcherptr,bool b){
  if (b){
    watched=true;
    watchptr=watcherptr;}
  else{
    watched=false;
    watchptr=0;
  }
}

void Parameter::killmonitor(){
  if ((ismonitored())&&(monitorptr!=0)){
      //be very careful here
      try{
          std::string dummy=monitorptr->getheaderstring();
      }
      catch(...){
          //didn't work
          monitorptr=0;
          return;
      }      
    delete(monitorptr);//this warns also the watched component and sets watchptr to 0
    #ifdef PARAMETER_DEBUG
      std::cout << "Killing my own monitor\n";
    #endif
  }
  
  
  if ((iswatched())&&(watchptr!=0)){
      //be very careful here
      try{
          std::string dummy=watchptr->getname();
      }
      catch(...){
          //didn't work
          watchptr=0;
          return;
      }
	#ifdef PARAMETER_DEBUG
      std::cout << "Killing the monitor of :"<<watchptr->getname()<<":\n";
    #endif      
    watchptr->killmonitor();  //this warns owner of the monitor and sets monitorptr to 0    
  }
}

void Parameter::setmonitor(Monitor* mptr,bool b){
	if (b){	   	
		//attach a monitor to this parameter
		monitorptr=mptr;
		monitored=true;		
		try{
			//be carefull, test it first
			monitorptr->getresult();
		}
		catch(...){
			//bad pointer
			monitorptr=0;
			monitored=false;
		}  
	}
	else{
		//forget about the monitor
		monitorptr=0;
		monitored=false;
	}  	
}

Monitor* Parameter::getmonitor()const{
  //return the monitor pointer if monitored
  if (monitored) return monitorptr;
  else return 0;
  }
bool Parameter::ismonitored()const{
  return monitored;
}

bool Parameter::iswatched()const{
  return watched;
}

Parameter Parameter::operator+(const Parameter p1){
  this->setvalue(this->getvalue()+p1.getvalue());
  return *this;
}
Parameter Parameter::operator-(const Parameter p1){
  this->setvalue(this->getvalue()-p1.getvalue());
  return *this;
}
Parameter Parameter::operator*(const Parameter p1){
  this->setvalue(this->getvalue()*p1.getvalue());
  return *this;
}
Parameter Parameter::operator/(const Parameter p1){
  this->setvalue(this->getvalue()/p1.getvalue());
  return *this;
}

void Parameter::setlinear(bool b){
    linear=b;
}
  bool Parameter::islinear()const{
    return linear;
}
