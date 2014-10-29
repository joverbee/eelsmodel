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
 * eelsmodel - components/spinsplitedge.h
 *
 * A spin splitted edge with white lines, can be a L2,3 or M4,5 edge
 * built from two hartree slater cross sections and two lorentzians.
 **/

#ifndef SPINSPLITEDGE_H
#define SPINSPLITEDGE_H

#include <fstream>

#include "src/core/component.h"

class SpinsplitEdge : public Component  {
Spectrum tempspectrum; //temporary storage of the spectrum shape
std::string filename; //the HS-GOS table filename
Component* continuum1; //pointer to HS cross section of the first continuum part
Component* continuum2; //pointer to HS cross section of the second continuum part
Component* whiteline1; //pointer to lorentzian white lines
Component* whiteline2; //pointer to lorentzian white lines

public:
//constructors
SpinsplitEdge();
SpinsplitEdge(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~SpinsplitEdge();

//modifiers
void calculate();
Spectrum* getgradient(size_t j){return 0;}
SpinsplitEdge* clone()const;
SpinsplitEdge* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
