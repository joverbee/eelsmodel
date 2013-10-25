/***************************************************************************
                          drudelorentz.h  -  description
A drude lorentz model for low loss eels
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

#ifndef DRUDELORENTZ_H
#define DRUDELORENTZ_H

#include <complex>

#include "src/core/component.h"
#include "src/core/curvematrix.h"

class Imagedisplay;

class DrudeLorentz : public Component  {
    size_t nrofextraparams;
    size_t nosc; //number of lorentz oscillators
    Spectrum* eps1spectrum; //storage for eps1 and ep2 spectrum
    Spectrum* eps2spectrum;
    size_t thetapoints;
    CurveMatrix kroegermatrix;
    Imagedisplay* kroegerim;
    bool showkroeger;
    bool integrate;
    bool dokroeger;
    Parameter* optionparamptr;

public:
DrudeLorentz();
DrudeLorentz(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~DrudeLorentz();
void calculate();
Spectrum* getgradient(size_t j){return 0;};
DrudeLorentz* clone()const;
DrudeLorentz* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
double getloss(double eps1,double eps2)const;
void addlorentzparams(size_t i);
void changenosc(int d);
void setoptions();
double Pefl(double theta,double thetaE,double v,double beta,std::complex<double> ec,double ev,double dE,double me,double t,bool onlyvolume);
double getlossKroeger(double eps1,double eps2,double t,double E00,double thetamax,size_t i,bool onlyvolume);
void displayKroeger();
void interactiveoptions();
void  getoptions();

};

#endif
