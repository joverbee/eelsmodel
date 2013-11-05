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
 * eelsmodel - components/gdos.h
 *
 * Generic Density of states multiplier.
 * This component creates a density of states profile that will multiply
 * a cross section to better mimic the real shape of an EELS excitation edge.
 * This function is 0 before Estart and 1 after Estart in between the function
 * is defined by n values.
 *
 * several options can be chosen for the function:
 *  - linear interpolation
 *  - cubic interpolation
 *  - upsampling
 *  - non-equidistant energy points
 *  - enforcing Bethe sum rule
 *
 * This class replaces all older DOSses and combines them in 1 component
 **/

#ifndef GDOS_H
#define GDOS_H

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>

#include "src/core/component.h"
#include "src/core/fftw++.h"

using namespace fftwpp;

class Monitor;

class GDos : public Component  {
size_t degree; //degree of the polynomial
 Component* compptr; //pointer to component that we are multiplying
 Spectrum* cumsumrule;
 Monitor* mptr; //pointer to monitor for mass center
 Monitor* mptr2; //pointer to monitor for area under edge
 rcfft1d* fft_planSUB; //fftw plan for forward fft
 crfft1d* ifft_planSUB; //fftw plan for inverse fft
 rcfft1d* fft_plan; //fftw plan for forward fft
 crfft1d* ifft_plan; //fftw plan for inverse fft
 double* realSUB; //real input copy of subspace
 Complex* fourierSUB; //complex output FFT of subspace
 double* real; //real input copy of upsampled equalisation function
 Complex* fourier; //complex output FFT of ''
 double subdispersion;
 size_t nsubspace;
size_t pointsbefore;
size_t pointsafter;
bool dosumrule;
size_t interpolationtype;
double smoothvalue;
double threshold;
std::vector<double> Evector; //a vector with energy positions of the different points
  Spectrum* Plotspec; //a spectrum containing the points to fit the spline trough for plotting purposes
  std::vector<double> Yvector; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> b; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> c; //a vector containing a copy of the parameter values for passing to spline routine
  std::vector<double> d; //a vector containing a copy of the parameter values for passing to spline routine
int offset;
bool linear;
Spectrum* dummy;
 //the gsl spline workspace
  gsl_interp_accel *acc;
  gsl_spline *sp;
  enum broadening{EGERTON,QUADRATIC,LINEAR,CONSTANT};
  broadening broadeningtype;


public:
GDos();
GDos(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~GDos();
void calculate();
Spectrum* getgradient(size_t j);
bool checkxsection();
GDos* clone()const;
GDos* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void gdosumrule(double &withgdos, double &withoutgdos);
void showsumrule();
void rescaleavg();
void preparesubspace();
void setoptions(); //virtual class that will call a user interface to choose the options
void initDOS();
std::string interactiveoptions();
double getoptions();
void storeoptions(std::string optionstring);
void makeoptions();
void changedegree(size_t olddegree,size_t degree);
double Lifetimebroadening(double E);
double masscenter();
void InitEnergy();
void copyparameters();
double seval(double u);
void dospline();
void gslinit();
void initplotspec(); //copy Yvector and Evector in the plotted spectrum
void show(); //overwrite the show from component
void orderchanged(); //overwrite orderchanged from component
void fitpowerlaw(double&A,double&r,size_t startindex,size_t stopindex);
void updatemonitors();
};

#endif
