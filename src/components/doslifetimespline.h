/***************************************************************************
                          doslifetimespline.h  -  description
Density of states multiplier
this component creates a desnity of states profiel that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by n values
which are interpolated with a quadratic spline
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

#ifndef DOSLIFETIMESPLINE_H
#define DOSLIFETIMESPLINE_H

//#define COMPONENT_DEBUG
#include <src/core/component.h>
#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

class Spectrum;

/**
  *@author Jo Verbeeck
  */
class DosLifetimeSpline : public Component  {
  size_t degree; //degree of the polynomial
  std::vector<double> Evector; //a vector with energy positions of the different points
  Spectrum* Plotspec; //a spectrum containing the points to fit the spline trough for plotting purposes
  std::vector<double> Yvector; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> b; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> c; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> d; //a vector containing a copy of the parameter values for passing to spline routine
int offset;
bool linear;
  Component* compptr; //pointer to component that we are multiplying
  //#ifdef COMPONENT_DEBUG
  Spectrum* dummy;
  //#endif

  //the gsl spline workspace
  gsl_interp_accel *acc;
  gsl_spline *sp;
  enum broadening{EGERTON,QUADRATIC,LINEAR,CONSTANT};
  broadening broadeningtype;
 public:
  DosLifetimeSpline();
 DosLifetimeSpline(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
 ~DosLifetimeSpline();
 void calculate();
 double  Lifetimebroadening(double E);

 void Warp(Spectrum* L);
 void InitEnergy();
 Spectrum* getgradient(size_t j);

 bool checkxsection();
 DosLifetimeSpline* clone()const;
 DosLifetimeSpline* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
 void copyparameters();
 double seval(double u);
 void dospline();
 void gslinit();
 void initplotspec(); //copy Yvector and Evector in the plotted spectrum
 void show(); //overwrite the show from component
 void orderchanged(); //overwrite orderchanged from component
};

#endif
