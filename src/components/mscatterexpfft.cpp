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
 * eelsmodel - components/mscatterexpfft.cpp
 **/

#include "src/components/mscatterexpfft.h"

#include <cmath>
#include <iostream>
#include <vector>

#include "src/core/eelsmodel.h"
#include "src/core/fftw++.h"
#include "src/core/multispectrum.h"
#include "src/core/parameter.h"

#include "src/gui/componentselector.h"
#include "src/gui/getgraphptr.h"
#include "src/gui/gettopspectrum.h"
#include "src/gui/saysomething.h"

using namespace fftwpp;

Eelsmodel* geteelsmodelptr();
QWorkspace* getworkspaceptr();

MscatterExpfft::MscatterExpfft()
:Component()
{
this->setname("Fourier Log Multiple scattering (use for low loss)");
this->setdescription("Self Convolution of a LL spectrum as used in fourier log deconvolution");
this->setcanconvolute(false); //meaningless in this case
this->setconvolutor(true); //makes this a special component!
setshifter(false);
}
MscatterExpfft::MscatterExpfft(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
Parameter* p1;
Parameter* p2;
compptr=0;
 //give a name and description
 this->setpppc(1.0); //test
 this->setname("Fourier Log Multiple scattering (use for low loss)");
 this->setcanconvolute(false); //meaningless in this case
 this->setconvolutor(true); //makes this a special component!
 setshifter(false);
 //get a const pointer to the HL spectrum (we don't want to change the spectrum)
Model* mymodel=geteelsmodelptr()->getmodel_nonconst();

if (parameterlistptr==0){
//create a parameter tlambda
    p1=new Parameter("Relative Thickness (t/lambda)",0.5,1);
    p1->interactivevalue("Enter relative thickness (t/lambda)");
    //parameter 2 points to the ZL peak

    int cnr=0;
   //create a componentselector here
    Componentselector myinput2(0,"","Select the ZL component",cnr);
    //get a pointer to this component
    compptr=mymodel->getcomponent(cnr);
    //save this number in a parameter
    p2=new Parameter("compnr",cnr,1);
    p2->setchangeable(false);
   }
 else{//get the parameter from the list
  	p1=(*parameterlistptr)[0];
  	p2=(*parameterlistptr)[1];
  	const int cnr=int(p2->getvalue());
    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
 }
this->addparameter(p1);
this->addparameter(p2);
//get info from the ZL peak

if (compptr==0){
      //something went wrong
      Saysomething mysay(0,"Error","the component didn't appear to be a valid ZL");
      throw Componenterr::unable_to_create();
}
//set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
//compptr->setmultiplierptr(this);


 //init FFTW for this size of spectra
 //pad=n-1; //ideal size of padding, maybe n works faster because power of 2
 pad=0; //no padding at the moment, low loss should go down anyway

 //create in and out buffer, out=complex
 realsize=n+pad;
 complexsize=realsize/2+1;
 realLL = FFTWdouble(realsize);
 fourierLL =FFTWComplex(complexsize);
realZL = FFTWdouble(realsize);
 fourierZL =FFTWComplex(complexsize);

 //make a plan for convolution
 fft_planLL=new rcfft1d(realsize,realLL,fourierLL);
 ifft_planLL=new crfft1d(realsize,fourierLL,realLL);
 fft_planZL=new rcfft1d(realsize,realZL,fourierZL);
 ifft_planZL=new crfft1d(realsize,fourierZL,realZL);

}

MscatterExpfft::~MscatterExpfft(){
  //delete storage and plans
  FFTWdelete(realLL);
  FFTWdelete(fourierLL);
  FFTWdelete(realZL);
  FFTWdelete(fourierZL);
  if (compptr!=0){
 //   compptr->releasemultiplier();
  }

}
void MscatterExpfft::calculate()
{
     const Model* mymodel=(geteelsmodelptr())->getmodel();
    if (!checkZL()) return;




 //check if compptr still exists, otherwise we should also die
 int shift=0;
 shift=compptr->getmaxindex(); //shift ZL peak to 1st pixel
  copytobuffer(mymodel,compptr,realLL,realZL,shift); //copy LL and ZL to fft buffer without the ZL, pad zeros, apply shift

  double t=this->getparameter(0)->getvalue();

  //perform fft
  fft_planLL->fft(realLL,fourierLL);//fft(LL)
  fft_planZL->fft(realZL,fourierZL); //fft(ZL)


  //Do exponential fourier series
  for (size_t i=0;i<(size_t)complexsize;i++){
    fourierLL[i]=multiply(fourierZL[i],exp(-t)*(exponential(fourierLL[i],t))); //feed normalised into exponential
    //ZL*[exp(-t)exp(s(e))] with s(e) normalised to 1 (without ZL peak and with delta function in zero, but shifted so that first pixel is E=0)
  }

  //now transform back to real space
  ifft_planLL->fftNormalized(fourierLL,realLL);

  //and copy the valid part of the spectrum to this spectrum
  //but correct for the shift that we applied earlier
  for (size_t i=0;i<(this->getnpoints());i++){
      size_t id=i+shift;
      if (id>=this->getnpoints()){
          id=id-this->getnpoints();
      }
    this->setcounts(id,realLL[i]);//readjust for shift
  }

}


Complex  MscatterExpfft::exponential(const Complex a1, double t)const
{
  //complex multiplication, for some reason the * operator in the complex class seems to produce nonsense
  //exp(-t a1)=exp(-t re(a1))exp(-t i im(a1))
  //re= exp(-t re(a1)) cos(-t*im(a1))
  const double rea1=a1.real();
  const double ima1=a1.imag();
  const double prefact=exp(t*rea1);
  return Complex(prefact*cos(t*ima1),prefact*sin(t*ima1));
}

Complex   MscatterExpfft::multiply(const Complex a1, const Complex a2)const
{
  //complex multiplication, for some reason the * operator in the complex class seems to produce nonsense
  const double rea1=a1.real();
  const double ima1=a1.imag();
  const double rea2=a2.real();
  const double ima2=a2.imag();
  return Complex(rea1*rea2-ima1*ima2,rea1*ima2+ima1*rea2);
}

void MscatterExpfft::copytobuffer(const Spectrum* spec, const Spectrum* ZL,double* buff,double* buffZL,int shift){
 //copy spectrum to input buffer for fft but subtract ZL peak,
  //apply zero padding to the right with "pad" nr of zeros
  //then shift the spectrum over "shift" points
  //buffer should be size N+pad
  //spectrum should be size N
  //finally normalise to 1
  //same for ZL, but not normalisation
  const int N=spec->getnpoints();

  for (int i=0;i<realsize;i++){
    if ((i+shift)<N){
      buff[i]=spec->getcounts(i+shift)-ZL->getcounts(i+shift);
      buffZL[i]=ZL->getcounts(i+shift);
    }
    else if ((i+shift)<realsize){
      buff[i]=0;
      buffZL[i]=0;
    }else{
      buff[i]=spec->getcounts(i+shift-realsize)-ZL->getcounts(i+shift-realsize);
      buffZL[i]=ZL->getcounts(i+shift-realsize);
    }
  }


    //normalise buff
    double norm=0.0;
    for (int i=0;i<realsize;i++){
        norm+=buff[i];
    }

    for (int i=0;i<realsize;i++){
        buff[i]=buff[i]/norm;
    }


}

bool MscatterExpfft::checkZL(){
  //check if the ZL component we are pointing to has still the same number
  //otherwise update the component number parameter
  Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
  const int cnr=mymodel->getcomponentindexbypointer(compptr);
  if (compptr==NULL){
    return false;
  }
  Parameter* cnrptr= getparameter(1);
  if ((cnr==-1)&&(compptr!=NULL)){
    //component not found also kill ourselves...
    Saysomething mysay(0,"Error","Fourier Log: the component we are pointing to seems to be not present");
    compptr=NULL;
    cnrptr->setchangeable(true); //unlock
    cnrptr->setvalue(-1); //update non-existing value
    cnrptr->setchangeable(false); //lock
    return false;
  }
  cnrptr->setchangeable(true); //unlock
  cnrptr->setvalue(cnr); //update to current value
  cnrptr->setchangeable(false); //lock
  return true;
}

MscatterExpfft* MscatterExpfft::clone()const{
return new MscatterExpfft(*this);}
MscatterExpfft* MscatterExpfft::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new MscatterExpfft(n,estart,dispersion,parameterlistptr);
 }
