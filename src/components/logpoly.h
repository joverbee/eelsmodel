/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * eelsmodel - components/logpoly.h
 *
 * Log polynomial background component.
 **/

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
