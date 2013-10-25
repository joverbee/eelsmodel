/***************************************************************************
                          fast_BG.h  -  description
Power law backround component
                             -------------------
    begin                : Sat Oct 26 2002
    copyright            : (C) 2002 by Jo Verbeeck
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
