/***************************************************************************
                          Mscatterfft.cpp  -  description
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

#include "src/components/mscatterfft.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>
#include "src/gui/getgraphptr.h"
#include "src/gui/gettopspectrum.h"
#include "src/gui/saysomething.h"
#include <vector>
#include "src/core/eelsmodel.h"
#include "src/core/multispectrum.h"
#include "src/core/fftw++.h"
using namespace fftwpp;



#include <QWorkspace>
//get two global pointers from main
extern Eelsmodel* geteelsmodelptr();
extern QWorkspace* getworkspaceptr();

Mscatterfft::Mscatterfft()
:Component(),LLspectrum()
{
this->setname("Multiple scattering (FFT)");
this->setdescription("Convolution of the HL spectrum with LL using fast fourier transform convolution.\nThis simulates the effect of multiple scattering\nwhich is an important effect in experimental spectra ");
this->setcanconvolute(false); //meaningless in this case
this->setconvolutor(true); //makes this a special component!
setshifter(false);
}
Mscatterfft::Mscatterfft(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),LLspectrum(n,estart,dispersion)
{
Parameter* p;
 //give a name and description
 this->setpppc(1.0); //test
 this->setname("Multiple scattering (FFT)");
 this->setdescription("Convolution of the HL spectrum with LL using fast fourier transform convolution.\nThis simulates the effect of multiple scattering\nwhich is an important effect in experimental spectra ");
 this->setcanconvolute(false); //meaningless in this case
 this->setconvolutor(true); //makes this a special component!
 setshifter(false);
 //get a const pointer to the HL spectrum (we don't want to change the spectrum)
 const Model* mymodel=(geteelsmodelptr())->getmodel();
 HLptr=0;
 HLptr=mymodel->getconstspectrumptr(); //pointer to the calculated model spectrum
 if (HLptr==0) throw Componenterr::unable_to_create();


 LLptr=0;
 multiLLptr=0;
 //get a pointer to a low loss spectrum (MULTI or normal)
 //in case no parameter list is given (=a new component)
 if (parameterlistptr==0){
  //get a pointer to a LL spectrum
  QWorkspace* myworkspace=getworkspaceptr();
  Getgraphptr* LLgetptr=new Getgraphptr(myworkspace,"","Make sure the LL spectrum is frontmost");
	if (LLgetptr==0){
	//stop creating this component
	Saysomething mysay(0,"Error","the LL spectrum didn't appear to be valid");
	throw Componenterr::unable_to_create();
	}

  LLgetptr->show();
  if ( LLgetptr->exec() == QDialog::Accepted ) {
    if (LLgetptr->ismulti()){
      multiLLptr=LLgetptr->getmultipointer();
      if (multiLLptr==0) throw Componenterr::unable_to_create();
      //check if both are same size
      if (multiLLptr->getnpoints()!=HLptr->getnpoints()){
        Saysomething mysay(0,"Error","Both spectra should be of the same size");
        throw Componenterr::unable_to_create();
        }
      }
    else{
      //convoluting with a singe LL spectrum
      LLptr=LLgetptr->getpointer();
	  if (LLptr==0){
		Saysomething mysay(0,"Error","The topmost window was not a spectrum");
        throw Componenterr::unable_to_create();
	  }
    }
  }else{
	  //the dialog got closed in another way
	  throw Componenterr::unable_to_create();
  }
  if (LLgetptr!=0) delete LLgetptr;
  if (multiLLptr==0){
    p=new Parameter(LLptr->getfilename(),0.0,false);//store the filename in a dummy parameter
    }
  else{
    std::string fname=multiLLptr->getfilename();
    //Saysomething mysay(0,"Error",QString::fromStdString(fname));
    p=new Parameter(fname,0.0,false);//store the filename in a dummy parameter
    }
  }


 //in case a parameterlist is given, load the spectrum file and fill in the parameters
 else{//get the parameter from the list
  p=(*parameterlistptr)[0];

  //let spectrum take care of finding the spectrum if the filename is wrong
  //the finale filename can be retrieved from the spectrum

    //get the edge file via the filename
  //first check wether this file can be found
  //Fileopener f(0,0,filename);
  //filename=f.open();
  //filename is now an updated name
  //if (filename=="") throw Componenterr::unable_to_create();//cancelled by user
  //p.setname(filename); //store this new name in the parameter


  //assume that if model is multispectrum also LL will be a multispectrum
  if (!mymodel->ismulti()){
    loadmsa l;
    LLptr = new Spectrum(l,p->getname()); //the filename is stored in the name of p
    if (LLptr==0) throw Componenterr::unable_to_create();
    }
  else{
    //ask eelsmodel to load a multispectrum
    multiLLptr=(geteelsmodelptr())->openDM(p->getname(),true); //open but silent
    if (multiLLptr==0) {
      //maybe it was a single spectrum after all...
      loadmsa l;
      LLptr = new Spectrum(l,p->getname()); //the filename is stored in the name of p
      if (LLptr==0) throw Componenterr::unable_to_create();}
    }
 }

 //init FFTW for this size of spectra
 pad=n-1; //ideal size of padding, maybe n works faster because power of 2
 //create in and out buffer, out=complex
 realsize=n+pad;
 complexsize=realsize/2+1;
 realLL = FFTWdouble(realsize);
 fourierLL =FFTWComplex(complexsize);
 realHL = FFTWdouble(realsize);
 fourierHL =FFTWComplex(complexsize);

 //make a plan for convolution
 fft_planLL=new rcfft1d(realsize,realLL,fourierLL);
 fft_planHL=new rcfft1d(realsize,realHL,fourierHL);
 ifft_planHL=new crfft1d(realsize,fourierHL,realHL);
 ifft_planLL=new crfft1d(realsize,fourierLL,realLL);


updateLL(); //set the LLptr to the right current spectrum, copy it and and normalise and do the fourier transform
 p->setname(LLptr->getfilename()); //update p to the real filename that was used
this->addparameter(p);
}
Mscatterfft::~Mscatterfft(){
  //delete storage and plans
  FFTWdelete(realLL);
  FFTWdelete(realHL);
  FFTWdelete(fourierLL);
  FFTWdelete(fourierHL);
}
void Mscatterfft::calculate()
{
  if (multiLLptr!=0){
    //if a multispectrum, check if the current slice has changed
    //if so, do an updateLL to get a new, normalised LL spectrum
    try{
      const Model* mymodel=(geteelsmodelptr())->getmodel();
      const Multispectrum* HLmulti=mymodel->getconstmultispectrumptr();
      if(multiLLptr->getcurrentslice()!=HLmulti->getcurrentslice()){
        updateLL();
      }
    }
    catch(...){
      throw Componenterr::unable_to_calculate();
    }
  }
  //do the FFT convolution
  copytobuffer(HLptr,realHL,0); //copy HL to fft buffer, pad zeros, apply no shift

  //perform fft
  fft_planHL->fft(realHL,fourierHL);

  //multiply both results in fourier domain and store in fourierHL
  for (size_t i=0;i<(size_t)complexsize;i++){
    fourierHL[i]=multiply(fourierHL[i],fourierLL[i]);
  }

  //now transform back to real space
  ifft_planHL->fftNormalized(fourierHL,realHL);

  //and copy the valid part of the spectrum to this spectrum
  for (size_t i=0;i<(this->getnpoints());i++){
    this->setcounts(i,realHL[i]);
    //LLspectrum.setcounts(i,(realHL[i]));
  }
  //LLspectrum.display(getworkspaceptr());
}
void Mscatterfft::updateLL(){
  if (multiLLptr!=0){
      //set the current spectrum of the LL to the same as the HL current spectrum
      const Model* mymodel=(geteelsmodelptr())->getmodel();
      const Multispectrum* HLmulti=mymodel->getconstmultispectrumptr();
      multiLLptr->setcurrentslice((HLmulti->getcurrentslice()));
      //and get the pointer to the current spectrum
      LLptr=multiLLptr->getcurrentspectrum();
   }
  //get a copy of the LL spectrum
  if (LLptr!=0){
    LLspectrum=(*LLptr);//copy the spectrum via the copy assignment defined in Spectrum class
    }
  else throw Componenterr::unable_to_create();




  //make sure the spectrum starts with y=0 and ends with y=0
  //use linear interpolation between both end
  //this is required when CCD problems occured
  //normally this should not be nessecary
  // E X P E R I M E N T A L
  const double starty=LLspectrum.getcounts(0);
  const double estart=LLspectrum.getenergy(0);
  const int n=LLspectrum.getnpoints();
  const double endy=LLspectrum.getcounts(n-1);
  const double eend=LLspectrum.getenergy(n-1);
  const double slope=(endy-starty)/(eend-estart);
  const double cliplimit=-20.0;
for (int i=0;i<n;i++){
    const double E=LLspectrum.getenergy(i);
    const double cts=LLspectrum.getcounts(i);
    //E X P E R I M E N T
    const double correction=starty+(E-estart)*slope;
    LLspectrum.setcounts(i,cts-correction);
    //clip off any remaining counts that are too negative
    if (cts-correction<cliplimit){
      LLspectrum.setcounts(i,cliplimit);
    }
  }


   //show this spectrum as a test
   //Spectrum* myspec=new Spectrum(n);
   //copy because LLspectrum will die when out of scope
   //(*myspec)=LLspectrum;
   //myspec->display(0);

  //normalize the spectrum to 1
  LLspectrum.normalize();

   //check if the maximum of the spectrum is close to 0eV energy, otherwise callibration is wrong
  ZLindex=LLspectrum.getmaxindex();
  double e0=LLspectrum.getenergy(ZLindex);
  if (fabs(e0)>10.0){
      //careful, for really thick specimen this fails and we should keep the calibrated
    Saysomething mysay(0,"warning","The position of the zero loss peak is more then 10eV off (should be close to 0eV), keeping calibrated units");
    ZLindex=LLspectrum.getenergyindex(0.0);
  }
  copytobuffer(&LLspectrum, realLL, ZLindex); //copy spectrum into fft buffer, pad zeros and shift

  //perform fft
  fft_planLL->fft(realLL,fourierLL);
  //result is now stored in fourierLL

/*
  //test do backward transform
  ifft_planLL->fftNormalized(fourierLL,realLL);
  for (size_t i=0;i<(this->getnpoints());i++){
    LLspectrum.setcounts(i,realLL[i]);
  }
  LLspectrum.display(getworkspaceptr());
*/
}

Complex  Mscatterfft::multiply(const Complex a1, const Complex a2)const
{
  //complex multiplication, for some reason the * operator in the complex class seems to produce nonsense
  const double rea1=a1.real();
  const double ima1=a1.imag();
  const double rea2=a2.real();
  const double ima2=a2.imag();
  return Complex(rea1*rea2-ima1*ima2,rea1*ima2+ima1*rea2);
}

void Mscatterfft::copytobuffer(const Spectrum* spec, double* buff,int shift){
 //copy spectrum to input buffer for fft,
  //apply zero padding to the right with "pad" nr of zeros
  //then shift the spectrum over "shift" points
  //buffer should be size N+pad
  //spectrum should be size N
  const int N=spec->getnpoints();
  for (int i=0;i<realsize;i++){
    if ((i+shift)<N){
      buff[i]=spec->getcounts(i+shift);
    }
    else if ((i+shift)<realsize){
      buff[i]=0;
    }else{
      buff[i]=spec->getcounts(i+shift-realsize);
    }
  }
}

Mscatterfft* Mscatterfft::clone()const{
return new Mscatterfft(*this);}
Mscatterfft* Mscatterfft::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Mscatterfft(n,estart,dispersion,parameterlistptr);
 }
