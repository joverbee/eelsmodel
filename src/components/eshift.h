/***************************************************************************
                          eshift.h  -  description

                          A component that simulates energy shift
                          It simply takes the calculated model and shifts it in energy
                          via linear interpolation

                             -------------------
    begin                : Fri 20 -2 2004
    copyright            : (C) 2004 by Jo Verbeeck
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

#ifndef ESHIFT_H
#define ESHIFT_H

#include <src/core/component.h>
class Model;
/**
  *@author Jo Verbeeck
  */

class Eshift : public Component  {


Spectrum* HLptr; //pointer to the HL spectrum
Model* mymodel; //pointer to model

public:
Eshift();
	Eshift(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Eshift();
void calculate();
Spectrum* getgradient(size_t j){return 0;}
Eshift* clone()const;
Eshift* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
