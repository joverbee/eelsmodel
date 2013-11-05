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
 * eelsmodel - components/mscatterfft.h
 **/

#ifndef MSCATTERFFT_H
#define MSCATTERFFT_H

//#define MSCATTERFFT_DEBUG

#include <vector>

#include "src/core/component.h"
#include "src/core/fftw++.h"

class Spectrum;
class Multispectrum;

using namespace fftwpp;

class Mscatterfft : public Component  {
int ZLindex; //index of the ZL peak position in the LL spectrum
Spectrum LLspectrum; //a COPY of the LL spectrum
const Spectrum* LLptr; //pointer to the real LL spectrum
Multispectrum* multiLLptr; //pointer to a LL multispectrum
const Spectrum* HLptr; //pointer to the HL spectrum
rcfft1d* fft_planLL; //fftw plan for forward fft
rcfft1d* fft_planHL; //fftw plan for forward fft
crfft1d* ifft_planHL; //fftw plan for inverse fft
crfft1d* ifft_planLL; //fftw plan for inverse fft
double* realLL; //real input copy of LL
Complex* fourierLL; //complex output FFT of LL
double* realHL; //real input of HL model spectrum
Complex* fourierHL;//complex output of FFT of HL model
 int pad; //amount of zeros padded to right to avoid artefacts due to circular convolution
 int complexsize;
 int realsize;
public:
Mscatterfft();
	Mscatterfft(int,double,double,std::vector<Parameter*>* parameterlistptr=0);
	~Mscatterfft();
void calculate();
 Spectrum* getgradient(size_t j){return 0;}
void updateLL();
 Complex  multiply(const Complex a1, const Complex a2)const;
 void copytobuffer(const Spectrum* spec, double* buff,int shift);
Mscatterfft* clone()const;
Mscatterfft* new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr=0)const;
};

#endif
