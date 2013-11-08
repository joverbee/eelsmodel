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

#include <cstdlib>

#include <Eigen/Core>

#include "src/fitters/fitter.h"
#include "src/core/model.h"

/* Maximum Likelihood fitter for Poisson Statistics. */
const int N = 100;

class LevenbergMarquardt : public Fitter
{
public:
  LevenbergMarquardt(Model* m);

  double goodness_of_fit()const;
  std::string goodness_of_fit_string()const;
  double likelihoodfunction()const; //the real likelihood function

  void CRLB(); //calculate with Cramer Rao Lower Bound the sigmas of the parameters
  double getcovariance(int i, int j); //get values from the covariance matrix, set it up first with preparecovariance
  void preparecovariance(); //calculate the covariance matrix from the fischer information matrix
  double likelihoodratio();
  void updatemonitors(); //update the covariances for each parameter that has a monitor

  enum method_enum{QR,inversion};

  //overloaded functions
  void createmodelinfo();
  void modified_partial_derivative(size_t j,const Spectrum* currentspectrum);
  void calculate_ModifiedJacobian();
  double calcstep(double lambda,method_enum method);
  void preparestep(method_enum method);
  double iteration();
  void prepareforiteration();
  void lin_from_nonlin();
  void storecurrentparams();
  void restorecurrentparams();
  void calcscaling();
  void calculate_dtprime();
  void calculate_Y();

private:
  std::size_t lambdaiter; //number of iterations for lambda
  double lambdac; //alpha starting step
  double nu; //factor with which to reduce lambda during itterations
  bool dolin;
  Eigen::MatrixXd Xprime;
  Eigen::MatrixXd XTX;
  Eigen::MatrixXd XTXcopy;
  Eigen::MatrixXd Q;
  Eigen::MatrixXd R;
  Eigen::MatrixXd Work;
  Eigen::MatrixXd Step;
  Eigen::MatrixXd dtprime;
  Eigen::MatrixXd Ga;
  Eigen::MatrixXd GaTGa;
  Eigen::MatrixXd Work2;
  Eigen::MatrixXd Y;
  Eigen::MatrixXd B;

  std::vector<double> x0;
  std::vector<double> x0nonlin;
  std::vector<double> d0;
};

#endif
