/***************************************************************************
                        ledge.h  -  description
Hydrogenic L-edge component based on SigmaL2 by R.F. Egerton
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

#ifndef LEDGE_H
#define LEDGE_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Ledge : public Component  {
Spectrum tempspectrum;
public:
Ledge();
	Ledge(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Ledge();
 void calculate();
Spectrum* getgradient(size_t j);
double getcorrfactor(double E,double alpha,double beta,double E0); //correction factor for alpha due to Isaacson and Scheinfein
double getcorrfactorKohl(double alpha,double beta,double theta);
Ledge* clone()const;
Ledge* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void estimateparams(); //estimate strength automatically
void setoptions();
};

#endif
