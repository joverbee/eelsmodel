/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * eelsmodel is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with eelsmodel.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Author: Jo Verbeeck, Ruben Van Boxem
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - components/gaussian.h
 *
 * A gaussian peak component.
 **/

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
