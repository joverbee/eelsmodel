/***************************************************************************
                          edgefile.h  -  description
A component which gets it spectrum from an existing spectrum, or loads it via the filename
The spectrum is automaticaly interpolated to the right energy dispersion of the model
The spectrum can be shifted, energy scaled, and amplitude scaled.
Preferable the edge file contains an EELS edge that has its onset at 0eV
Use this componet to load experimental edges or calculated edges for example from ab initio calculations
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

#ifndef EDGEFILE_H
#define EDGEFILE_H

#include "src/core/component.h"

class EdgeFile : public Component  {	
private:
//member data
Spectrum edge; //a copy of the loaded edge
Spectrum tempspectrum;
public:
EdgeFile();
EdgeFile(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~EdgeFile();

void calculate();
Spectrum* getgradient(size_t j){return 0;}

EdgeFile* clone()const;
EdgeFile* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
