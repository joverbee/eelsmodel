/***************************************************************************
                          parameter.h  -  description
                             -------------------
    begin                : Sat Oct 12 2002
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

#ifndef PARAMETER_H
#define PARAMETER_H

//#define PARAMETER_DEBUG

#include<string>

class Monitor;

class Parameter {
//class data
  double value;
  double sigma;
  bool isschanged;
  bool changeable;
  bool bound;
  bool coupled;
  double lowerbound;
  double upperbound;
  std::string name;
  Parameter* coupled_parameter;
  bool master;
  bool monitored;
  bool watched;
  bool displayed;
  bool linear; //is this parameter a linear parameter in a model?
  bool warn; //switch warning on boundary of or on
  double couple_fraction;
  Monitor* monitorptr; //a pointer to a monitor that watches this parameter
  Parameter* watchptr; //a pointer to a parameter that is watching you via a monitor
//member functions
public:
  Parameter();
  Parameter(double val);
  Parameter(std::string namestr,double val);
  Parameter(std::string namestr,double val,bool changeallowed);
  ~Parameter();
Parameter(const Parameter&) ; //discourage copying
  Parameter& operator=(const Parameter&);

  void setdefaults();
  void interactivevalue(std::string description); //interactive setup of the value;
  void  setvalue(double x);
  void  setdisplayed(bool b);
  bool isdisplayed()const;
  void  setsigma(double x);
  void  setunchanged();
  void setchanged();
  void forcechanged();
  double getvalue()const;
  double getsigma() const;
  bool setboundaries(double x,double y);
  void setupperbound(double u);
  void setlowerbound(double l);
  bool boundaryOK(double x,double up,double low) const;
  double getupperbound() const;
  double getlowerbound() const;
  void  setchangeable(bool b);

  void setlinear(bool b);
  bool islinear()const;

  void setcoupled(bool b);
  void setbound(bool b);
  bool changed() const;
  bool ischangeable() const;
  bool isbound() const;
  bool iscoupled() const;
  bool iscoupledslave()const{return ((iscoupled())&&(isslave()));};
  Parameter* getmaster() const;
  void setname(std::string namestr);
  std::string getname() const;//get name string including "coupled to:" in case of coupling
  std::string getpurename() const;//get the real name
  void couple(Parameter* p,double fraction);
  void deletecoupling();
  double getcouplingfactor(){return couple_fraction;};
  void setmonitor(Monitor* mptr,bool b=0);
  Monitor* getmonitor()const;
  bool ismonitored()const;
  bool iswatched()const;
  void setwatched(Parameter* watcherptr,bool b);
  Parameter operator+(const Parameter p1);
  Parameter operator-(const Parameter p1);
  Parameter operator*(const Parameter p1);
  Parameter operator/(const Parameter p1);
  void killmonitor();
  void setmaster(Parameter* slavepar);  
  void endcoupling();
private:  
   void setslavepointer(Parameter* p){coupled_parameter=p;};
   bool ismaster()const {return master;};
   bool isslave()const  {return !master;};


};

//to do
// define operator >> and << for writing and reading parameter data to/from files and stdout/in

#endif
