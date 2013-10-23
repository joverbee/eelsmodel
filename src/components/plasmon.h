/***************************************************************************
                          plasmon.h  -  description
A simple plasmon model to model the LL spectrum
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

#ifndef PLASMON_H
#define PLASMON_H

#include <src/core/component.h>
class Lorentz;
/**
  *@author Jo Verbeeck
  */

class Plasmon : public Component  {
int nrofplasmons; //number of plasmons that are simulated
std::vector<Lorentz*> peaklist; //a vectro with pointers to the lorentz peaks


public:
Plasmon();
	Plasmon(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Plasmon();
 void calculate();
Spectrum* getgradient(size_t j){return 0;};
Plasmon* clone()const;
Plasmon* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
private:
double poissonfraction(double tlambda,int n)const;
};

#endif
