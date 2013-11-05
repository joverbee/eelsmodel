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
 * eelsmodel - components/kedge.h
 *
 * Hydrogenic K-edge component based on SigmaK by R.F. Egerton.
 **/

#ifndef KEDGE_H
#define KEDGE_H

#include "src/core/component.h"

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
