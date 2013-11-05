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
 * Copyright: 2005-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - components/fowler.h
 *
 * A Fowler Nordheim peak component to model a zero loss peak of a FEG.
 **/

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
