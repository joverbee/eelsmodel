/***************************************************************************
                          fowler.h  -  description
A Fowler Nordheim peak component to model a zero loss peak of a FEG
                             -------------------
    begin                : Mon May 09 2005
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

#ifndef FOWLER_H
#define FOWLER_H

#include "src/core/component.h"

class Fowler : public Component  {
public:
Fowler();
Fowler(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
Fowler(int n,double estart,double dispersion,double epeak,double height,double d, double T);
~Fowler();
void calculate();
Spectrum* getgradient(size_t j);
 double FNfunction(double energy, double d,double T);
Fowler* clone()const;
Fowler* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
