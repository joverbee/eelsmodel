/***************************************************************************
                          dieleclinear.h  -  description
A dielectric model for low loss eels using a piecewise linear model for the loss function
                             -------------------
    begin                : Sun Dec 5 2008
    copyright            : (C) 2008 by Jo Verbeeck
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

#ifndef DIELECLINEAR_H
#define DIELECLINEAR_H

#include "src/core/component.h"
#include <complex>
/**
  *@author Jo Verbeeck
  */

#include "src/core/curvematrix.h"
class Imagedisplay;


class DielecLinear : public Component  {
    size_t nrofextraparams;
    size_t degree; //number of lorentz oscillators
    Spectrum* eps1spectrum; //storage for eps1 and ep2 spectrum
    Spectrum* eps2spectrum;
    size_t thetapoints;
    CurveMatrix kroegermatrix;
    Imagedisplay* kroegerim;
    bool showkroeger;
    bool integrate;
    bool dokroeger;
    bool dorel;
    int outputformat;
    Parameter* optionparamptr;
    std::vector<double> avec;
    std::vector<double>   bvec;
    std::vector<double>  en1vec;
    std::vector<double>  en2vec;
    std::vector<double>  en1sqvec;
    std::vector<double>   en2sqvec;

public:
DielecLinear ();
DielecLinear (int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~DielecLinear ();
void calculate();
Spectrum* getgradient(size_t j){return 0;};
DielecLinear * clone()const;
DielecLinear * new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
//double getloss(double eps1,double eps2,double qmax,double qE)const;
void addlinparams(size_t i);
void changedegree(int d);
void setoptions();
double Pefl(double theta,double thetaE,double v,double beta,std::complex<double> ec,double ev,double dE,double me,double t,bool onlyvolume);
double LossRel(std::complex<double> eps,double E,double q,const std::complex<double> betasq,double qE);
double getlossKroeger(double eps1,double eps2,double t,double E00,double thetamax,size_t i,bool onlyvolume);
void displayKroeger();
void interactiveoptions();
void getoptions();
void show(); //redefine what to do at show
void estimateparams(); //autofitter
};

#endif
