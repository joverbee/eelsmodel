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
 * eelsmodel - components/mscatter.cpp
 **/

#include "src/components/mscatter.h"

#include <cmath>
#include <iostream>
#include <vector>

#include <QWorkspace>

#include "src/core/parameter.h"
#include "src/core/multispectrum.h"

#include "src/gui/eelsmodel.h"
#include "src/gui/getgraphptr.h"
#include "src/gui/gettopspectrum.h"
#include "src/gui/saysomething.h"

QWorkspace* getworkspaceptr();
EELSModel* geteelsmodelptr();

Mscatter::Mscatter()
:Component(),LLspectrum()
{
name = "Multiple scattering (matrix convolution)";
this->setdescription("Convolution of the HL spectrum with LL using matrix convolution.\nThis simulates the effect of multiple scattering\nwhich is an important effect in experimental spectra ");
this->setcanconvolute(false); //meaningless in this case
this->setconvolutor(true); //makes this a special component!
setshifter(false);
}
Mscatter::Mscatter(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),LLspectrum(n,estart,dispersion)
{
  /*
Parameter* p;
 //give a name and description
 this->setpppc(1.0); //test
 name = "Multiple scattering (matrix convolution)";
 this->setdescription("Convolution of the HL spectrum with LL using matrix convolution.\nThis simulates the effect of multiple scattering\nwhich is an important effect in experimental spectra ");
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
 QWorkspace* myworkspace= getworkspaceptr();
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
  delete LLgetptr;
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
    LLptr = new Spectrum(p->getname()); //the filename is stored in the name of p
    if (LLptr==0) throw Componenterr::unable_to_create();
    }
  else{
    //ask eelsmodel to load a multispectrum
    multiLLptr=(geteelsmodelptr())->openDM3(p->getname(),true); //open but silent
    if (multiLLptr==0) {
      //maybe it was a single spectrum after all...
      LLptr = new Spectrum(p->getname()); //the filename is stored in the name of p
      if (LLptr==0) throw Componenterr::unable_to_create();}
    }
 }
updateLL(); //set the LLptr to the right current spectrum, copy it and and normalise
 p->setname(LLptr->getfilename()); //update p to the real filename that was used
this->addparameter(p);
 */
}
Mscatter::~Mscatter(){
}
void Mscatter::calculate()
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
//do the matrix convolution
double cts=0.0;
int npoints=this->getnpoints();
  for (int i=0;i<npoints;i++)
  {
    cts=0.0;
    for(int j=-ZLindex;((j<=i)&&(j<(npoints-ZLindex)));j++){
      #ifdef MSCATTER_DEBUG
      if ((ZLindex+j)<0) {
	std::cout<<"ZLindex+j= negative! "<<(ZLindex+j)<<"\n";
	}
      if ((i-j)<0) {
	std::cout<<"i-j= negative!"<<(i-j)<<"\n";
	}
      if ((ZLindex+j)>=npoints) {
	std::cout<<"ZLindex+j>=npoints! "<<(ZLindex+j)<<"\n";
	}
      //if ((i-j)>=npoints) {std::cout<<"i-j>= npoints!"<<i-j<<"\n";}
      #endif
      if ((i-j)<npoints){
        cts+=LLspectrum.getcounts(ZLindex+j)*HLptr->getcounts(i-j);
        }
      }
    this->setcounts(i,cts);
  }

}
void Mscatter::updateLL(){
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


   //check if the maximum of the spectrum is close to 0eV energy, otherwise callibration is wrong
  ZLindex=LLspectrum.getmaxindex();
  double e0=LLspectrum.getenergy(ZLindex);
  if (fabs(e0)>10.0){
    //Saysomething mysay(0,"warning","The position of the zero loss peak is more then 10eV (should be close to 0eV), I will correct this");
  }

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



}

Mscatter* Mscatter::clone()const{
return new Mscatter(*this);}
Mscatter* Mscatter::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Mscatter(n,estart,dispersion,parameterlistptr);
 }
