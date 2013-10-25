/***************************************************************************
                          mlfitter.h  -  description
                             -------------------
    begin                : Sat Nov 23 2002
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
double likelyhoodfunction()const; //the real likelyhood function
void calculate_beta_and_alpha();
void CRLB(); //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double getcovariance(int i,int j); //get values from the covariance matrix, set it up first with preparecovariance
void preparecovariance(); //calculate the covariance matrix from the fischer information matrix
double likelihoodratio();
void updatemonitors(); //update the covariances for each parameter that has a monitor
};

#endif
