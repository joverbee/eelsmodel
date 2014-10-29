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
 * Copyright: 2008-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel  - components/dieleclinear.h
 *
 * A dielectric model for low loss eels using a piecewise linear loss function.
 **/

#ifndef DIELECLINEAR_H
#define DIELECLINEAR_H

#include <complex>

#include <Eigen/Core>

#include "src/core/component.h"


class Imagedisplay;

class DielecLinear : public Component  {
    size_t nrofextraparams;
    size_t degree; //number of lorentz oscillators
    Spectrum* eps1spectrum; //storage for eps1 and ep2 spectrum
    Spectrum* eps2spectrum;
    size_t thetapoints;
    Eigen::MatrixXd kroegermatrix;
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
