/**
 * This file is part of eelsmodel.
 *
 * eelsmodel is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
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
 * eelsmodel - fitters/levenbergmarquadt.h
 **/

#ifndef LEVENBERGMARQUARDT_H
#define LEVENBERGMARQUARDT_H

//#define FITTER_DEBUG //print some debug info

#include "src/fitters/fitter.h"
#include "src/core/model.h"
#include "src/core/curvematrix.h"

/* Maximum Likelihood fitter for Poisson Statistics. */
#define N 100

class LevenbergMarquardt : public Fitter  {
    size_t lambdaiter; //number of iterations for lambda
    double lambdac; //alpha starting step
    double nu; //factor with which to reduce lambda during itterations
    bool dolin;
    CurveMatrix Xprime;
    CurveMatrix XprimeT;
    CurveMatrix XTX;
    CurveMatrix XTXcopy;
    CurveMatrix Q;
    CurveMatrix R;
    CurveMatrix Work;
    CurveMatrix Step;
    CurveMatrix dtprime;
    //CurveMatrix Ga;
    //CurveMatrix GaT;
    //CurveMatrix GaTGa;
    //CurveMatrix Work2;
    CurveMatrix Y;
    CurveMatrix Yprime;
    CurveMatrix B;
bool prepared;

    std::vector<double> x0;
    std::vector<double> d0;
    enum method_enum{QR,inversion,linear};

public:
LevenbergMarquardt(Model* m);
~LevenbergMarquardt();
double goodness_of_fit()const;
std::string goodness_of_fit_string()const;
double likelyhoodfunction()const; //the real likelyhood function

void CRLB(); //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double getcovariance(int i,int j); //get values from the covariance matrix, set it up first with preparecovariance
void preparecovariance(); //calculate the covariance matrix from the fischer information matrix
double likelihoodratio();
void updatemonitors(); //update the covariances for each parameter that has a monitor

//overloaded functions
void createmodelinfo();
void modified_partial_derivative(size_t j,const Spectrum* currentspectrum);
void calculate_ModifiedJacobian();
double calcstep(double lambda,method_enum method);
void preparestep(method_enum method);
double iteration();
void prepareforiteration();
void storecurrentparams();
void restorecurrentparams();
void calcscaling();
void calculate_dtprime();
void calculate_Y();
double weight(size_t j);
};

#endif
