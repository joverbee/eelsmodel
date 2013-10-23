/***************************************************************************
                          mscatter.h  -  description
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

#ifndef MSCATTER_H
#define MSCATTER_H

#include <src/core/component.h>
#include <vector>
class Spectrum;
class Multispectrum;
//#define MSCATTER_DEBUG
/**
  *@author Jo Verbeeck
  */

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
