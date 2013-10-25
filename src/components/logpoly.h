/***************************************************************************
                          logpoly.h  -  description
Log polynomial background component
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

#ifndef LOGPOLY_H
#define LOGPOLY_H

#include "src/core/component.h"

class Logpoly : public Component  {
size_t degree; //degree of the polynomial
public:
Logpoly();
	Logpoly(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Logpoly();
 void calculate();
Spectrum* getgradient(size_t j);
Logpoly* clone()const;
Logpoly* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
