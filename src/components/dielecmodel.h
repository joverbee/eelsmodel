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
 * Copyright: 2002-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - components/dielecmodel.h
 *
 * A dielectric model for low loss EELS
 **/

#ifndef DIELECMODEL_H
#define DIELECMODEL_H

#include <complex>

#include <Eigen/Dense>

#include "src/core/component.h"

class Imagedisplay;

class DielecModel : public Component  {
    size_t nrofextraparams;
    size_t nosc; //number of lorentz oscillators
    Spectrum* eps1spectrum; //storage for eps1 and ep2 spectrum
    Spectrum* eps2spectrum;
    size_t thetapoints;
    Eigen::MatrixXd kroegermatrix;
    Imagedisplay* kroegerim;
    bool showkroeger;
    bool integrate;
    bool dokroeger;
    Parameter* optionparamptr;

public:
DielecModel ();
DielecModel (int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~DielecModel ();
void calculate();
Spectrum* getgradient(size_t j){return 0;};
DielecModel * clone()const;
DielecModel * new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
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
