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
 * eelsmodel components/dos.h
 *
 * Density of states multiplier.
 * This component creates a density of states profile that will multiply
 * a cross section to better mimic the real shape of an EELS excitation edge.
 * This function is 0 before Estart and 1 after Estart. In between the
 * function is defined by n values which are linearly interpolated.
 **/

#ifndef DOS_H
#define DOS_H

#include "src/core/component.h"
#include "src/core/fftw++.h"

using namespace fftwpp;

class Dos : public Component  {
size_t degree; //degree of the polynomial
 Component* compptr; //pointer to component that we are multiplying
 Spectrum* cumsumrule;
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

public:
Dos();
Dos(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
~Dos();
void calculate();
Spectrum* getgradient(size_t j);
bool checkxsection();
Dos* clone()const;
Dos* new_component(int,double,double,std::vector<Parameter*>* parameterlistptr=0)const;
void dosumrule(double &withdos, double &withoutdos);
void showsumrule();
void rescaleavg();
void preparesubspace();
};

#endif
