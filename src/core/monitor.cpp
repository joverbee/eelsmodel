/***************************************************************************
                          monitor.cpp  -  description
                             -------------------
    begin                : Fri Aug 8 2003
    copyright            : (C) 2003 by Jo Verbeeck
    email                : joverbee@ruca.ua.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
//#define MONITOR_DEBUG
#include "src/core/monitor.h"
#include <string>
#include <iostream>
#include <sstream>
#include <cmath>
#include <cstdio>
#ifndef linux
//#include <stdio>
#endif
#include <iostream>





// make use of
// gamma=expression, like param1-param2 or param1+param2 or param1/param2
//theta=[param1 param 2]
// dgamma/dtheta=[dgamma/dparam1 dgamma/dparam2]   2x1 matrix
//
// var(gamma)=dgamma/dtheta COV dgamma/dtheta`
// with COV 2x2 the covariance matrix (inverse Fischer information matrix)




Monitor::Monitor(Parameter* parp1,Parameter* parp2,int typeindex){
  //init the monitor
  headerstring="";
  value=0.0;
  pp1=parp1;
  pp2=parp2;
  //tell it to the main parameter
  pp1->setmonitor(this,true);
  //notify pp2 that you are watching him
  if (typeindex!=3) pp2->setwatched(pp1,true); //makes no sense for 3
  type=typeindex;
  cov=0.0;
#ifdef MONITOR_DEBUG
  std::cout << "New monitor created\n";
  std::cout <<"parameter1: "<<pp1->getname()<<" parameter2: "<<pp2->getname()<<" type:"<<typeindex<<"\n";
#endif
}
Monitor::~Monitor(){
  //release pp1
  pp1->setmonitor(0,false);
  //release pp2
  if (type!=3) pp2->setwatched(0,false);
  #ifdef MONITOR_DEBUG
  std::cout << "Monitor killed\n";
  #endif
}
double Monitor::difference()const{
  return (pp1->getvalue()-pp2->getvalue());
}
double Monitor::sum()const{
  return (pp1->getvalue()+pp2->getvalue());
}
double Monitor::ratio()const{
  const double eps=1.0e-20;
  if (fabs(pp2->getvalue())>eps){ //avoid division by zero
  return (pp1->getvalue()/pp2->getvalue());
  }
  else{
    return 0.0;
  }
}
double Monitor::getresult()const{
  switch (type){
    case 0:
    return difference();
    case 1:
    return sum();
    case 2:
    return ratio();
    case 3:
    return value;
    default:
    return 0.0;
  }
}

void Monitor::setheaderstring(std::string s){
    //only valid for type 3
    headerstring=s;
    }
void Monitor::setvalue(double d){
    //only valid for type 3
    value=d;
}

std::string Monitor::getheaderstring()const{
    //return string for use in headers with a list of parameters
    std::ostringstream s;
    switch (type){
    case 0:
        if (s << "Difference of "<< pp2->getname()<< "and "<<pp2->getname()){
            // conversion worked
            return s.str();
        }
        break;
    case 1:
        if (s << "Sum of "<< pp2->getname()<< "and "<<pp2->getname()){
            // conversion worked
            return s.str();
        }
        break;

    case 2:
        if (s << "Ratio of "<< pp2->getname()<< "and "<<pp2->getname()){
            // conversion worked
            return s.str();
        }
        break;
    case 3:
        return headerstring;
    break;
    default:
      return "unknown monitor type";
    }
    return "unknown monitor type";
}

double Monitor::getsigma()const{
  switch (type){
    case 0:
    return differencesigma();
    case 1:
    return sumsigma();
    case 2:
    return ratiosigma();
    default:
    return 0.0;
  }
}
double Monitor::differencesigma()const{
  //calculate the stddev of a difference of two parameters taking into account covariance
  double sig1=pp1->getsigma();
  double sig2=pp2->getsigma();
  double var;
  try{
    var=pow(sig2,2.0)+pow(sig1,2.0)-2.0*cov;
  }
  catch(...){
    return 0.0;
  }
  if (var<=0.0) return 0.0;
  return sqrt(var);
}
double Monitor::sumsigma()const{
  //calculate the stddev of a sum of two parameters taking into account covariance
  double sig1=pp1->getsigma();
  double sig2=pp2->getsigma();
  double var;
  try{
    var=pow(sig2,2.0)+pow(sig1,2.0)+2.0*cov;
  }
  catch(...){
    return 0.0;
  }
  if (var<=0.0) return 0.0;
  return sqrt(var);
}
double Monitor::ratiosigma()const{
  //calculate the stddev of a ratio of two parameters taking into account covariance
  double sig1=pp1->getsigma();
  double sig2=pp2->getsigma();
  double val1=pp1->getvalue();
  double val2=pp2->getvalue();
  double var;
  const double eps=1.0e-20;
  try{
    if (fabs(val2)>eps){
    //avoid division by zero crash...doesn't throw an exception so try doesn't really help here
    var=pow(sig1/val2,2.0)+pow(val1*sig2/pow(val2,2.0),2.0)-(val1/pow(val2,3.0))*2.0*cov;
    }
    else var=0.0;
  }
  catch(...){
    return 0.0;
  }
  if (var<=0.0) return 0.0;
  return sqrt(var);
}
std::string Monitor::getresultstring()const{

  //check wether pp2 and pp1 still exists
  try{
    //getvalue doesnt crash on a bad pointer
    if (type!=3){
    	const bool b2=pp2->isbound();
    	pp2->setbound(false);
    	pp2->setbound(b2);
    }   
  }
  catch(...){
    //remove this monitor
    pp1->killmonitor();
    return "";
  }
  std::ostringstream s;
  //add header value and sigma
  if (s << getresult()<< " +- "<< getsigma() << " " <<getheaderstring()){
            // conversion worked
            return s.str();
  }
  return ""; //in case conversion didn't work
}
std::string Monitor::getresultstringonlyvalues()const{
    //return only the values, not the header as a string
    //check wether pp2 and pp1 still exists
  try{
    //getvalue doesnt crash on a bad pointer
	  if (type!=3){
	      	const bool b2=pp2->isbound();
	      	pp2->setbound(false);
	      	pp2->setbound(b2);
	      }     
  }
  catch(...){
    //remove this monitor
    pp1->killmonitor();
    return "";
  }
  std::ostringstream s;
  //add header value and sigma
  if (s << getresult()<< "\t"<< getsigma()){
            // conversion worked
            return s.str();
  }
  return ""; //in case conversion didn't work
}


int Monitor::gettype()const{
  return type;
}

