/***************************************************************************
                          pseudovoigt.h  -  description
A Pseudo-Voigt peak
                             -------------------
    begin                : Sun Oct 27 2002
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

#ifndef PSEUDOVOIGT_H
#define PSEUDOVOIGT_H

#include "src/core/component.h"

class PseudoVoigt : public Component  {
    Component * Lorentzptr;
    Component * Gaussptr;
public:
PseudoVoigt();
PseudoVoigt(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~PseudoVoigt();
void calculate();
double getarea();
PseudoVoigt* clone()const;
PseudoVoigt* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
Spectrum* getgradient(size_t j);
};

#endif
