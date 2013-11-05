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
 * eelsmodel - components/fixedpattern.h
 *
 * A component which gets it spectrum from an existing spectrum, or loads it
 * via the filename. The spectrum is automaticaly interpolated to the right
 * energy dispersion of the model. The spectrum can be shifted, energy scaled,
 * and amplitude scaled. Preferably the edge file contains an EELS edge that
 * has its onset at 0 eV. Use this componet to load experimental edges or
 * calculated edges for example from ab initio calculations.
 **/

#ifndef FIXEDPATTERN_H
#define FIXEDPATTERN_H

#include "src/core/component.h"

class FixedPattern : public Component  {
private:
//member data
Spectrum edge; //a copy of the loaded edge

public:
FixedPattern();
FixedPattern(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~FixedPattern();

void calculate();
Spectrum* getgradient(size_t j){return 0;}

FixedPattern* clone()const;
FixedPattern* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
