/***************************************************************************
                          powerlaw.h  -  description
Power law backround component
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

#ifndef POWERLAW_H
#define POWERLAW_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Powerlaw : public Component  {
public:
Powerlaw();
	Powerlaw(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Powerlaw();
 void calculate();
Spectrum* getgradient(size_t j);
Powerlaw* clone()const;
Powerlaw* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void fitpowerlaw(double&A,double&r,size_t startindex,size_t stopindex); //obtain A and r param from a fit
void estimateparams();
void setoptions();
};

#endif
