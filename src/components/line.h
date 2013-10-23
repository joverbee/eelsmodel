/***************************************************************************
                          line.h  -  description
                             -------------------
    begin                : Sun Oct 27 2002
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

#ifndef LINE_H
#define LINE_H

#include <src/core/component.h>

/**
  *@author Jo Verbeeck
  */

class Line : public Component  {
public:
Line();
	Line(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Line();
void calculate();
Spectrum* getgradient(size_t j){return 0;}
Line* clone()const;
Line* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
