/***************************************************************************
                          gaussian.h  -  description
A gaussian peak component
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

#ifndef GAUSSIAN_H
#define GAUSSIAN_H

#include <valarray>

#include "src/core/component.h"

class Gaussian : public Component  {
double eps;
public:
Gaussian();
Gaussian(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
Gaussian(int n,double estart,double dispersion,double epeak,double fwhm,double height);
	~Gaussian();
void calculate();
Spectrum* getgradient(size_t j);
Gaussian* clone()const;
Gaussian* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
