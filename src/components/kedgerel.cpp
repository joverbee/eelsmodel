/***************************************************************************
                          kedgerel.cpp  -  description
                  Relativistic Hydrogenic K-edge component

                                  based on

       "Relativistic ionisation cross sections for use in microanalysis"
Rainer Knippelmeier, Petra Wahlbring, Helmut Kohl, Ultramicroscopy 68 (1997) 25-41

                             -------------------
    begin                : Sat Feb 15 2003
    copyright            : (C) 2003 by Jo Verbeeck
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

#include "src/components/kedgerel.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>



Kedgerel::Kedgerel() //create a dummy version
:Component(),tempspectrum()
{
this->setname("Relativistic H K-edge");
this->setdescription("Relativistic H K-edge based on \n Relativistic ionisation cross sections for use in microanalysisi\nRainer Knippelmeier, Petra Wahlbring, Helmut Kohl\n Ultramicroscopy 68 (1997) 25-41");
}
Kedgerel::Kedgerel(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion)
{
//add the required parameters
Parameter* p1;
Parameter* p2;
Parameter* p3;
Parameter* p4;
Parameter* p5;
Parameter* p6;
if (parameterlistptr==0){
  p1=new Parameter("E0",300e3,1);
  p1->interactivevalue("enter primary energy [eV]");
  p1->setchangeable(false);  //the fitter should not change this in normal operation

  p2=new Parameter("Ek",estart,1);
  p2->interactivevalue("enter edge energy [eV]");

  p3=new Parameter("Z",8.0,1);
  p3->interactivevalue("enter atomic number Z");
  p3->setchangeable(false);  //the fitter should not change this in normal operation

  p5=new Parameter("thetamax",2.0e-3,1);
  p5->interactivevalue("enter collection half angle theta [rad]");
  p5->setchangeable(false); //the fitter should not change this in normal operation

  p4=new Parameter("thetasteps",100.0,1);
  p4->interactivevalue("enter number of steps for theta integration");
  p4->setchangeable(false); //the fitter should not change this in normal operation

  p6=new Parameter("strength",1.0e2,1);
  p6->interactivevalue("enter strength of edge");
}
else{
  p1=(*parameterlistptr)[0];
  p2=(*parameterlistptr)[1];
  p3=(*parameterlistptr)[2];
  p4=(*parameterlistptr)[3];
  p5=(*parameterlistptr)[4];
   p6=(*parameterlistptr)[5];
}
p6->setlinear(true); //strength is a linear parameter
this->addparameter(p1);
this->addparameter(p2);
this->addparameter(p3);
this->addparameter(p4);
this->addparameter(p5);
this->addparameter(p6);

//give a name and description
this->setname("Relativistic H K-edge");
this->setdescription("Relativistic H K-edge based on \n Relativistic ionisation cross sections for use in microanalysisi\nRainer Knippelmeier, Petra Wahlbring, Helmut Kohl\n Ultramicroscopy 68 (1997) 25-41");
this->setcanconvolute(true);
//we have a gradient wrt strength param
this->sethasgradient(5,true);
setshifter(false);
}
Kedgerel::~Kedgerel(){
}
void Kedgerel::calculate(){
  //get the parameters
  const Parameter* E0ptr= getparameter(0);
  const double E0=E0ptr->getvalue();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  const Parameter* Zptr= getparameter(2);
  const double Z=Zptr->getvalue();
  const Parameter* thetastepptr= getparameter(3);
  double thetasteps=thetastepptr->getvalue();
  const Parameter* thetamaxptr= getparameter(4);
  const double thetamax=thetamaxptr->getvalue();
  const Parameter* strengthptr= getparameter(5);
  const double strength=strengthptr->getvalue();



  //if parameters have changed, calculate again
  if (E0ptr->changed()||Ekptr->changed()||Zptr->changed()||thetastepptr->changed()||thetamaxptr->changed()){
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating K edge \n E0: " << E0 << " Ek:" <<Ek
            << " Z: "<<Z<<" thetasteps: "<<thetasteps<<" thetamax: "<<thetamax<<" strength: "<<strength<<"\n";
    #endif
    if (thetasteps<=1.0) thetasteps=100.0; //just for safety
    const double thetastep=thetamax/thetasteps;
    const double dispersion=this->getdispersion();
    //definition of fundamental constants
    const double e=1.602e-19; //electron charge in C
    const double m0=9.110e-31; //electron rest mass in kg
    const double a0=5.292e-11; //Bohr radius in m
    const double c=2.998e8; //speed of light in m/s
    const double hbar=1.055e-34; //Planck constant in J.s
    const double alfa=1.0/137.0; //fine structure constant
    const double Pi=acos(-1.0);

    //calculate relativistic correction factors
    const double gam=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
    const double T=E0*(1.0+gam)/(2.0*pow(gam,2.0)); //in eV
    const double beta=sqrt(1.0-1.0/pow(gam,2.0)); //relative speed of incoming electrons wrt. c
    const double v=beta*c; //speed of incoming electrons in m/s
    const double ki=gam*m0*v/hbar; //initial wave vector in 1/m

    //Reduce atomic number to account for electronic screening
    const double Zs=Z-0.3125;
    const double lambda=Zs/a0; //reciprocal orbit size 1/m
    const double lambdasq=pow(lambda,2.0);
    const double Ni=1.0/sqrt(1.0+0.25*pow(Zs*alfa,2.0) ); //normalisation factor for Darwin wave function
    const double prefactor=0.5*e*pow(alfa*ki,2.0)*4.0*m0/pow(beta*hbar,2.0); //prefactor for double differential cross section
    for (unsigned int i=0;i<(this->getnpoints());i++){//energy loop
      double cts=0.0;
      const double E=this->getenergy(i);
      double integral=0.0;

      //some more variables
      const double kf=sqrt(pow(ki,2.0)-2.0*gam*m0*E*e/pow(hbar,2.0)+pow(E*e,2.0)/pow(hbar*c,2.0)); //final wave vector 1/m
      const double thetaE=E/(2.0*gam*T); //characteristic scattering angle in rad
      const double kappasq=(2.0*m0/pow(hbar,2.0))*E*e-pow(lambda,2.0); //energy^2 of secondary electron (can be negative) in J^2
      const double kappa=sqrt(fabs(kappasq)); // |kappa|
      const double Nf=1.0/sqrt( 1.0+(pow(hbar*kappa,2.0))/(4.0*pow(m0*c,2.0))); //normalisation factor for Darwin wave function
      //create better scaled variables to avoid numerical errors
      const double kappal=kappa/lambda;
      const double kappalsq=kappasq/lambdasq;
      const double chi=lambda/kappa;
      const double preS=pow(Ni,2.0)*pow(Nf,2.0); //prefactor for S lambda^7/kappal factor missong
      if ((E>=Ek)&&(E>0.0)&&(Ek>0.0)){
        for (double theta=0.0;theta<=thetamax;theta+=thetastep){//collection angle loop
          const double qsq=pow(ki,2.0)*(pow(theta,2.0)+pow(thetaE,2.0));
          const double q=sqrt(qsq); //momentum transfer 1/m
          //create better scaled variables to avoid numerical errors
          const double qlsq=qsq/lambdasq;

          double Snorm=1.0;
          double gamma=0.0;
          double Y=0.0;
          //three different regions
          if(kappasq<=0.0){
          //if kappasq negative (energy is below ionisation treshold)
            gamma=-fabs(chi)*log((qlsq+1.0-kappalsq+2.0*kappal)/(qlsq+1.0-kappalsq-2.0*kappal));
            //log was absent in original paper, typo????
            Y=gamma;
            }
          else{
          //if kappasq is positive (energy is above ionisation treshold)
            if ((qlsq-kappalsq+1.0)>0.0){
              gamma=atan(2.0*kappal/(qlsq-kappalsq+1.0));
              }
            else{
              gamma=atan(2.0*kappal/(qlsq-kappalsq+1.0))+Pi;
              }
            Y=-2.0*gamma*chi; //another typo in the original paper?
            Snorm=1.0/(1.0-exp(-2.0*Pi*chi)); //extra normalisation
            }
          const double denom=pow(qlsq+kappalsq+1.0,2.0)-4.0*qlsq*kappalsq;
          const double S=Snorm*preS*exp(Y)/(3.0*pow(denom,3.0)); //lambda^-7 factor missing
          const double A=S*(
            1024.0*qlsq*(3.0*qlsq+kappalsq+1.0)
            +512.0*pow(lambda,2.0)*pow(hbar/(m0*c),2.0)*qlsq*((kappalsq+1.0)*(1.0+qlsq+2.0*kappalsq)-3.0*qlsq*kappalsq)
            +16.0*pow(lambda,4.0)*pow(hbar/(m0*c),4.0)*(
              3.0*(kappalsq+1.0)*(pow((1.0+qlsq+kappalsq),2.0)-4.0*qlsq*kappalsq)*(1.0+qlsq+kappalsq)
              -4.0*qlsq*kappalsq*(1.0+3.0*qlsq+kappalsq)
              )
            ); //factor lambda^-3 missing

          const double B=S*256.0*pow(hbar/(m0*c),2.0)*(
            (kappalsq+1.0)*(pow(1.0+qlsq+kappalsq,2.0)-4.0*qlsq*kappalsq)
            +pow(qlsq,2.0)*(1.0+3.0*qlsq+kappalsq)
            ); //factor lambda^-1 missing

          const double tscalbeta=beta*kf*sin(theta)/q; // t scalar with beta
          const double part1=(1.0/pow(lambda,6.0))*A*(1.0/pow(qlsq,2.0));
          const double part2=pow( tscalbeta/( qsq-pow(E*e/(hbar*c),2.0) ),2.0)*B;
          integral+=(part1+part2)*2.0*Pi*theta; //integrate over theta, take care of round aperture
          }//angle loop
        cts=prefactor*integral*thetastep*dispersion; //differential cross section
        }
      else{
          cts=0.0; //if not in the energy range
        }
      //multiply by 1e28 to give reasonable numbers
      cts=cts*1.0e28;

      tempspectrum.setdatapoint(i,E,cts,0.0);//store the shape in the temp spectrum
      this->setdatapoint(i,E,cts*strength,0.0);//then, multiply by the strength in the component spectrum
      }//end of energy loop

    }//end of if parameters changed

  else{
    if(strengthptr->changed()){
      //if only the strength has changed take the spectrum from the tempspectrum and multiply with strength
      //this is a lot faster and important for the fitting routines
      for (unsigned int i=0;i<(this->getnpoints());i++){
        const double E=this->getenergy(i);
        this->setdatapoint(i,E,tempspectrum.getcounts(i)*strength,0.0);
        }
      #ifdef COMPONENT_DEBUG
      std::cout <<"parameters have not changed, only strength has changed\n";
      #endif
      }
    #ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, do nothing\n";
    #endif
    }
    //set parameters as unchanged since last time we calculated
    this->setunchanged();
}

Spectrum* Kedgerel::getgradient(size_t j){
//get analytical partial derivative to strength parameter
//(the more we can avoid calculating these time consuming componets the better)

  switch(j){
  case 5:
  //analytical derivative wrt strength equals tempspectrum
    for (size_t i=0;i<this->getnpoints();i++)
  {
    gradient.setcounts(i,tempspectrum.getcounts(i));
  }
  break;

  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}
Kedgerel* Kedgerel::clone()const{
return new Kedgerel(*this);}
Kedgerel* Kedgerel::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Kedgerel(n,estart,dispersion,parameterlistptr);
}
