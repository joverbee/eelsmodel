/***************************************************************************
                          MscatterExpfft.h  -  description
                             -------------------
    begin                : Sun Oct 27 2002
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

#ifndef MSCATTEREXPFFT_H
#define MSCATTEREXPFFT_H

//#define MSCATTEREXPFFT_DEBUG

#include <vector>

#include "src/core/component.h"
#include "src/core/fftw++.h"

class Spectrum;
class Multispectrum;

using namespace fftwpp;

class MscatterExpfft : public Component  {
rcfft1d* fft_planLL; //fftw plan for forward fft
crfft1d* ifft_planLL; //fftw plan for inverse fft
rcfft1d* fft_planZL; //fftw plan for forward fft
crfft1d* ifft_planZL; //fftw plan for inverse fft
double* realLL; //real input copy of LL
Complex* fourierLL; //complex output FFT of LL
double* realZL; //real input copy of LL
Complex* fourierZL; //complex output FFT of LL
 int pad; //amount of zeros padded to right to avoid artefacts due to circular convolution
 int complexsize;
 int realsize;
 Component* compptr;
public:
MscatterExpfft();
	MscatterExpfft(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~MscatterExpfft();
void calculate();
 Spectrum* getgradient(size_t j){return 0;}
void updateLL();
 Complex  exponential(const Complex a1, const double t)const;
 Complex  multiply(const Complex a1, const Complex a2)const;
 void copytobuffer(const Spectrum* spec, const Spectrum* ZL,double* buff,double* buffZL,int shift);
MscatterExpfft* clone()const;
MscatterExpfft* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
bool checkZL();
};

#endif
