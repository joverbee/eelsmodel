/***************************************************************************
                         expbackground.h  -  description
Exponential background component
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

#ifndef EXPBACKGROUND_H
#define EXPBACKGROUND_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Expbackground : public Component  {
public:
Expbackground();
	Expbackground(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Expbackground();
 void calculate();
Spectrum* getgradient(size_t j){return 0;}
Expbackground* clone()const;
Expbackground* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
