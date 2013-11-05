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
 * eelsmodel - components/mscatter.h
 **/

#ifndef MSCATTER_H
#define MSCATTER_H

//#define MSCATTER_DEBUG

#include <vector>

#include "src/core/component.h"

class Spectrum;
class Multispectrum;

class Mscatter : public Component  {
int ZLindex; //index of the ZL peak position in the LL spectrum
Spectrum LLspectrum; //a COPY of the LL spectrum
const Spectrum* LLptr; //pointer to the real LL spectrum
Multispectrum* multiLLptr; //pointer to a LL multispectrum
const Spectrum* HLptr; //pointer to the HL spectrum

public:
Mscatter();
	Mscatter(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Mscatter();
void calculate();
Spectrum* getgradient(size_t j){return 0;}
void updateLL();
Mscatter* clone()const;
Mscatter* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
