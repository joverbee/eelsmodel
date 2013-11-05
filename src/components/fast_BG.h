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
 * eelsmodel - components/fast_BG.h
 *
 * Power law backround component.
 **/

#ifndef fast_BG_H
#define fast_BG_H

#include "src/core/component.h"

class fast_BG : public Component  {
public:
fast_BG();
    fast_BG(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
    ~fast_BG();
 void calculate();
Spectrum* getgradient(size_t j);
fast_BG* clone()const;
fast_BG* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void fitfast_BG(double&A,double&r,size_t startindex,size_t stopindex); //obtain A and r param from a fit
void estimateparams();
void setoptions();

};

#endif
