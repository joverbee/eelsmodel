/***************************************************************************
                         hsedge.h  -  description
Hartree Slater cross section component based on tabulated GOS tables by P. Rez
The tables are part of Gatan Digital Micrograph and are NOT GPL
This program only allows you to read the files if you already have them via a license
for DM.
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

#ifndef HSEDGE_H
#define HSEDGE_H

#include <src/core/component.h>
#include <fstream>
#include "src/core/curvematrix.h"
/**
  *@author Jo Verbeeck
  */

class HSedge : public Component  {
Spectrum tempspectrum; //temporary storage of the spectrum shape
std::string filename; //the HS-GOS table filename
float info1_1,info1_2,info1_3,info2_1,info2_2; //info numbers determining the energy and q scale
int ncol,nrow;
CurveMatrix* GOSmatrix; //HS gos table matrix

public:
//constructors
HSedge();
HSedge(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~HSedge();

//modifiers
void calculate();
Spectrum* getgradient(size_t j);
void readGOSfile(std::string filename);
std::string getmultiplatformline(std::ifstream*,int);//help function to read lines from unix,win or mac files

//inspectors
double getgosenergy(int i)const;
double getcorrfactor(double E,double alpha,double beta,double E0); //correction factor for alpha due to Isaacson and Scheinfein
double getcorrfactorKohl(double alpha,double beta,double theta);
double getgosq(int j)const;
double getinterpolatedgos(double energy,double q)const;
int filetype(std::string filename)const;
HSedge* clone()const;
HSedge* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
