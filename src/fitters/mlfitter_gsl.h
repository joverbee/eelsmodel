/***************************************************************************
                          mlfitter_gsl.h  -  description
                             -------------------
    begin                : Sat Nov 23 2002
    copyright            : (C) 2002 by Jo Verbeeck
    email                : jo@localhost.localdomain
 ***************************************************************************/

// A wrapper to use the GSL library for fitting, for more robust results

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef MLFITTERGSL_H
#define MLFITTERGSL_H

//#define FITTER_DEBUG //print some debug info

#include  <gsl/gsl_multifit_nlin.h>

#include "src/core/model.h"
#include "src/fitters/fitter.h"

/* Maximum likelihood fitter for Poisson Statistics. */

class MLFitterGSL : public Fitter  {
public:
gsl_multifit_function_fdf f;      //the function to be minimised
gsl_multifit_fdfsolver * solver;  //a pointer to the GSL solver
gsl_vector * initialparameters;   //storage of parameters
gsl_matrix * covar;               //storage for covariance matrix
Fitter* dummypar;                 //points to this as a workaround for the static functions
MLFitterGSL(Model* m);
~MLFitterGSL();
double goodness_of_fit()const;
double iteration();               //redefinition of itteration function, calls itteration of the GSL fitter
std::string goodness_of_fit_string()const;
double likelyhoodfunction()const; //Chi square
double likelyhoodfunctionML()const; //real likelyhood
void calculate_beta_and_alpha();  //empty since not needed
void CRLB();                      //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double getcovariance(const int i,const int j);//get values from the covariance matrix, set it up first with preparecovariance
void preparecovariance();         //calculate the covariance matrix from the fischer information matrix
double likelihoodratio();
void updatemonitors();            //update the covariances for each parameter that has a monitor
void convertxtoparam(const gsl_vector * x); //convert parameter vector x to our model parameters
double functionvalue(size_t i); //get the value of the function at index i, that needs to be optimised to zero
void initfitter();                //initialise new fitter and allocate space for parameter value vactor x, and covar matrix
private:
void initfdf();                   //function to be minimized and derivatives
void initparameters();            //copy current parameters of the model in vector x
void created_modelinfo();         //extra function excecuted after createmodelinfo(), used to tell the gsl solver that things have changed
};


//}
#endif
