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
 * eelsmodel - components/plasmon.h
 *
 * A simple plasmon model to model the LL spectrum.
 **/

#ifndef PLASMON_H
#define PLASMON_H

#include "src/core/component.h"

class Lorentz;

class Plasmon : public Component  {
int nrofplasmons; //number of plasmons that are simulated
std::vector<Lorentz*> peaklist; //a vectro with pointers to the lorentz peaks


public:
Plasmon();
	Plasmon(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Plasmon();
 void calculate();
Spectrum* getgradient(size_t j){return 0;};
Plasmon* clone()const;
Plasmon* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
private:
double poissonfraction(double tlambda,int n)const;
};

#endif
