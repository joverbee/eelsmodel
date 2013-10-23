/***************************************************************************
                         kedge.h  -  description
Hydrogenic K-edge component based on SigmaK by R.F. Egerton
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

#ifndef KEDGE_H
#define KEDGE_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Kedge : public Component  {
Spectrum tempspectrum;
public:
Kedge();
	Kedge(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Kedge();
 void calculate();
 double getcorrfactor(double E,double alpha,double beta,double E0); //correction factor for alpha due to Isaacson and Scheinfein
 double getcorrfactorKohl(double alpha,double beta,double theta); //correction factor for alpha due to H. Kohl
Spectrum* getgradient(size_t j);
Kedge* clone()const;
Kedge* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void estimateparams(); //estimate strength automatically
void setoptions();
};

#endif
