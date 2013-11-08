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
 * eelsmodel - fitters/mlfitter.h
 **/

#ifndef MLFITTER_H
#define MLFITTER_H

//#define FITTER_DEBUG //print some debug info

#include "src/fitters/fitter.h"
#include "src/core/model.h"

/* Maximum Likelihood fitter for Poisson Statistics. */

class MLFitter : public Fitter  {
public: 
MLFitter(Model* m);
~MLFitter();
double goodness_of_fit()const;
std::string goodness_of_fit_string()const;
double likelihoodfunction()const; //the real likelihood function
void calculate_beta_and_alpha();
void CRLB(); //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double getcovariance(int i,int j); //get values from the covariance matrix, set it up first with preparecovariance
void preparecovariance(); //calculate the covariance matrix from the fischer information matrix
double likelihoodratio();
void updatemonitors(); //update the covariances for each parameter that has a monitor
};

#endif
