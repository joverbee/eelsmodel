/***************************************************************************
                          doslifetime.h  -  description
Density of states multiplier
this component creates a desnity of states profiel that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by n values
which are linearly interpolated
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

#ifndef DOSLIFETIME_H
#define DOSLIFETIME_H
#define USELORENTZ
//choose between either Gauss or Lorentzian

//#define COMPONENT_DEBUG
#include <src/core/component.h>

class Lorentz;
class Gaussian;

//class Component;
/**
  *@author Jo Verbeeck
  */
class DosLifetime : public Component  {
  size_t degree; //degree of the polynomial
  #ifdef USELORENTZ
      std::vector<Lorentz*> peaklist; //a vector with pointers to the lorentz peaks
#endif
#ifndef USELORENTZ
    std::vector<Gaussian*> peaklist; //a vector with pointers to the lorentz peaks
#endif
  Component* compptr; //pointer to component that we are multiplying
  //#ifdef COMPONENT_DEBUG
  Spectrum* dummy;
  //#endif

 public:
  DosLifetime();
 DosLifetime(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
 ~DosLifetime();
 void calculate();
 double  Lifetimebroadening(double E);

 void Warp(Component* L);
 void InitLorentzians();
 Spectrum* getgradient(size_t j);

 bool checkxsection();
 DosLifetime* clone()const;
 DosLifetime* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
