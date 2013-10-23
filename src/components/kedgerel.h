/***************************************************************************
                         kedgerel.h  -  description
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

#ifndef KEDGEREL_H
#define KEDGEREL_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Kedgerel : public Component  {
Spectrum tempspectrum;
public:
Kedgerel();
	Kedgerel(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Kedgerel();
 void calculate();
Spectrum* getgradient(size_t j);
Kedgerel* clone()const;
Kedgerel* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
