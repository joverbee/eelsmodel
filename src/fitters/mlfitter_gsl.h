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
 * eelsmodel - fitters/mlfitter_gsl.h
 *
 * A wrapper to use the GSL nonlinear fitting routines to get a more robust
 * (but slower) result as a first test only least square, ML would require
 * small changes in the GSL code.
 **/

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
double likelihoodfunction()const; //Chi square
double likelihoodfunctionML()const; //real likelihood
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
