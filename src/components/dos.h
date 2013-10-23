/***************************************************************************
                          dos.h  -  description
Density of states multiplier
this component creates a desnity of states profiel that
will multiply a cross section to better mimic the real shape of an EELS
excitation edge.
This function is 0 before Estart
and 1 after Estart
in between the function is defined by n values
which are linearly interpolated
                             -------------------
    begin                : Sat Oct 26 2002
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

#ifndef DOS_H
#define DOS_H
#include "src/core/fftw++.h"
#include <src/core/component.h>

using namespace fftwpp;

//class Component;
/**
  *@author Jo Verbeeck
  */
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
