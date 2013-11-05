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
 * eelsmodel - components/ledge.cpp
 **/

#include "src/components/ledge.h"

#include <cmath>
#include <iostream>

#include "src/core/eelsmodel.h"
#include "src/core/parameter.h"

Eelsmodel* geteelsmodelptr();//main.cpp contains this global function with a pointer to eelsmodel

Ledge::Ledge() //create a dummy version
:Component(),tempspectrum()
{
this->setname("Hydrogenic L-edge");
this->setdescription("Hydrogenic L-edge based on SigmaL2 by R.F. Egerton");
}
Ledge::Ledge(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),tempspectrum(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  Parameter* p3;
  Parameter* p4;
  Parameter* p5;
  Parameter* p6;
  Parameter* p7;
  if (parameterlistptr==0){
    p1=new Parameter("E0",300e3,1);
    p1->interactivevalue("enter primary energy [eV]");
    p1->setchangeable(false);  //the fitter should not change this in normal operation
    p3=new Parameter("Z",13,1);
    p3->setboundaries(13.0,27.0); //Z needs to be in this range
    p3->interactivevalue("enter atomic number Z [>=13]");
    p3->setchangeable(false);  //the fitter should not change this in normal operation
    int index=int(p3->getvalue())-13;
    //list of approximate values for edge energy
    const double  IE3[] = {73.0,99.0,135.0,164.0,200.0,245.0,294.0,347.0,402.0,455.0,513.0,575.0,641.0,710.0,779.0,855.0,931.0,1021.0,1115.0,1217.0,1323.0,1436.0,1550.0,1675.0};
    if ((index<=24)&&(index>0)){
      p2=new Parameter("Ek",IE3[index],1);
    }
    else{
      p2=new Parameter("Ek",0.0,1);
    }
    p2->interactivevalue("enter edge energy [eV]");

    p5=new Parameter("collection angle",2.0e-3,1);
    p5->interactivevalue("enter collection half angle [rad]");
    p5->setchangeable(false); //the fitter should not change this in normal operation

    p7=new Parameter("convergence angle",0.0e-3,1);
    p7->interactivevalue("enter convergence half angle [rad]");
    p7->setchangeable(false); //the fitter should not change this in normal operation

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
    //try to see if we also find an convergence angle
    //to be compatible with older version
    if (parameterlistptr->size()==7){
        p7=(*parameterlistptr)[6];
    }
    else{
        //create a new convergence angle parameters and put it to 0.0
        p7=new Parameter("convergence angle",0.0e-3,1);
        //p7.interactivevalue("enter convergence half angle [rad]");
        p7->setchangeable(false); //the fitter should not change this in normal operation
    }
  }

  p6->setlinear(true); //strength is a linear parameter

  this->addparameter(p1);
  this->addparameter(p2);
  this->addparameter(p3);
  this->addparameter(p4);
  this->addparameter(p5);
  this->addparameter(p6);
  this->addparameter(p7);

  //give a name and description
  this->setname("Hydrogenic L-edge");
  this->setdescription("Hydrogenic L-edge based on SigmaL3 by R.F. Egerton");
  this->setcanconvolute(true);
  //we have a gradient wrt strength parameter
  this->sethasgradient(5,true);
  setshifter(false);
}
Ledge::~Ledge(){
}

void Ledge::calculate(){
  //check if any of paramters is changed, if not : don't calculate
  const Parameter* E0ptr= getparameter(0);
  const double E0=E0ptr->getvalue();
  const Parameter* Ekptr= getparameter(1);
  const double Ek=Ekptr->getvalue();
  const Parameter* Zptr= getparameter(2);
  const double Z=Zptr->getvalue();
  const Parameter* thetastepptr= getparameter(3);
  const double thetasteps=thetastepptr->getvalue();
  const Parameter* thetamaxptr= getparameter(4);
  const double thetamax=thetamaxptr->getvalue();
  const Parameter* strengthptr= getparameter(5);
  const double strength=strengthptr->getvalue();
  const Parameter* alphaptr= getparameter(6);
  const double alpha=alphaptr->getvalue();

  if (E0ptr->changed()||Ekptr->changed()||Zptr->changed()||thetastepptr->changed()||thetamaxptr->changed()||alphaptr->changed()){
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating L edge \n E0: " << E0 << " Ek:" <<Ek
            << " Z: "<<Z<<" thetasteps: "<<thetasteps<<" thetamax: "<<thetamax<<" strength: "<<strength<<"\n";
    #endif

    const double dispersion=this->getdispersion();
    //const double eps=1.0e-99;
    //definition of fundamental constants
    const double R=13.606; //Rydberg of energy in eV
    const double e=1.602e-19; //electron charge in C
    const double m0=9.110e-31; //electron rest mass in kg
    const double a0=5.292e-11; //Bohr radius in m
    const double c=2.998e8; //speed of light in m/s
    //const double hbar=1.055e-34; //Plank constant in J.s
    const double Pi=acos(-1.0);
    //calculate relativistic correction factors
    const double gamma=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
    const double T=E0*(1.0+gamma)/(2.0*pow(gamma,2.0)); //in eV
    //const double vsq=2.0*e*T/m0; //speed of electrons in field

    //Constants by Egerton
    //as in old eelsmodel
    //const double  XU[]={0.44,0.39,0.28,0.25,0.22,0.19,0.16,0.16,0.12,0.13,0.13,0.12,0.12,0.11,0.10,0.20,0.10,0.20};
    const double  XU[]={0.52,0.42,0.30,0.29,0.22,0.30,0.22,0.16,0.12,0.13,0.13,0.14,0.16,0.18,0.19,0.22,0.14,0.11,0.12,0.12,0.12,0.1,0.1,0.1};

    const double  IE3[] = {73.0,99.0,135.0,164.0,200.0,245.0,294.0,347.0,402.0,455.0,513.0,575.0,641.0,710.0,779.0,855.0,931.0,1021.0,1115.0,1217.0,1323.0,1436.0,1550.0,1675.0};
    const double  IE1[] = {118.0,149.0,189.0,229.0,270.0,320.0,377.0,438.0,500.0,564.0,628.0,695.0,769.0,846.0,926.0,1008.0,1096.0,1194.0,1142.0,1248.0,1359.0,1476.0,1596.0,1727.0};
    const int  index=int(Z)-13;
    const double  U=XU[index];
    const double  EL3=IE3[index];
    const double  EL1=Ek+(IE1[index]-EL3);//take relative distance from start


    //Reduce atomic number to account for electronic screening
    const double Zs=Z-0.35*7.0-1.7;

    for (unsigned int i=0;i<(this->getnpoints());i++){//energy loop
      const double E=this->getenergy(i);
      double integral=0.0;
      double cts=0.0;
      if ((E>=Ek)&&(E>0.0)){
        const double qa0sqmin=pow(E,2.0)/(4.0*R*T)+pow(E,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
        const double qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin((thetamax+alpha)/2.0),2.0);
        const double lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/floor(thetasteps);
        const double khsq=(E/(pow(Zs,2.0)*R))-0.25; //different expression compared to Kedge?


        const double akh=sqrt(fabs(khsq));
        const double RF=pow(((E+0.1-Ek)/(1.8*pow(Z,2.0))),U); //Correction factor for screening

        // collection angle loop, do on a logarithmic grid lnqa0sq
        for (double lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq+=lnqa0sqstep){
          //Angle loop
          const double qa0sq=exp(lnqa0sq);
          //Dimensionless variables
          const double Q=qa0sq/pow(Zs,2.0);
          //Calculate GOS
          double dfkdE=0.0;
          double C=0.0;
          double D=0.0;
          if (khsq>=0.0){
            double beta=atan(akh/(Q-khsq+0.25));
            if (beta<0.0) beta+=Pi;
            C=exp(-2.0*beta/akh);
            D=1.0-exp(-2.0*Pi/akh);
            }
          else{
            const double y=(-1.0/akh)*log((Q+0.25-khsq+akh)/(Q+0.25-khsq-akh));
            C=exp(y);
            D=1.0;
            }
          if (E<=EL1){
            const double G=2.25*pow(Q,4.0)-(0.75+3.0*khsq)*pow(Q,3.0)+(0.59375-0.75*khsq-0.5*pow(khsq,2.0))*pow(Q,2.0)+(0.11146+0.85417*khsq+1.8833*pow(khsq,2.0)+pow(khsq,3.0))*Q+0.0035807+khsq/21.333+pow(khsq,2.0)/4.5714+pow(khsq,3.0)/2.4+pow(khsq,4.0)/4.0;
            const double A=pow((pow((Q-khsq+0.25),2.0)+khsq),5.0);
            //dfkdE=RF*32.0*G*C*E/(A*D*R*pow(Zs,4.0)); //as it was
            dfkdE=RF*32.0*G*C*E/(A*D*R*R*pow(Zs,4.0)); //this is correct
            }
          else{
            const double G=pow(Q,3.0)-((5.0/3.0)*khsq+11.0/12.0)*pow(Q,2.0)+((pow(khsq,2.0))/3.0+1.5*khsq+65.0/48.0)*Q+(pow(khsq,3.0))/3.0+0.75*pow(khsq,2.0)+(23.0/48.0)*khsq+5.0/64.0;
            const double A=pow((pow(Q-khsq+0.25,2.0)+khsq),4.0);
            //dfkdE=RF*32.0*G*C*E/(A*D*R*pow(Zs,4.0)); as it was
            dfkdE=RF*32.0*G*C*E/(A*D*R*R*pow(Zs,4.0));
            }
            //integration over theta
            const double theta=2.0*sqrt(fabs(R*(qa0sq-qa0sqmin)/(4.0*pow(gamma,2.0)*T))); //fabs to avoid small neg number to cause probs with sqrt
            //see eq. 3.146 in Egerton
            integral+=getcorrfactorKohl(alpha,thetamax,theta)*dfkdE*lnqa0sqstep;
          }//end of collection angle loop
        cts= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E*T))*integral*dispersion; /* in m**2/eV */
        //try larger for better numerical behaviour
        //const double cts= (1.0/E)*integral*dispersion; /* not in m**2/eV */
        }//end of if E>Ek
      else{
        cts=0.0;
        }
      //multiply by 1e28 to give reasonable numbers
      cts=cts*1.0e28;
      tempspectrum.setdatapoint(i,E,cts,0.0);//store the shape in the temp spectrum
      this->setdatapoint(i,E,cts*strength,0.0);//then, multiply by the strength in the component spectrum
      }//end of energy loop

    }//end of if changed loop

  else{//if only strengthptr changed
    if(strengthptr->changed()){
      //if only the strength has changed take the spectrum from the tempspectrum and multiply with strength
      //this is a lot faster and important for the fitting routines
      for (unsigned int i=0;i<(this->getnpoints());i++){
        double en=this->getenergy(i);
        this->setdatapoint(i,en,tempspectrum.getcounts(i)*strength,0.0);
        }
      #ifdef COMPONENT_DEBUG
      std::cout <<"parameters have not changed, only strength has changed\n";
      #endif
      }
    }
  //set parameters as unchanged since last time we calculated
  this->setunchanged();
}
Spectrum* Ledge::getgradient(size_t j){
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
Ledge* Ledge::clone()const{
return new Ledge(*this);}
Ledge* Ledge::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Ledge(n,estart,dispersion,parameterlistptr);
}
double Ledge::getcorrfactor(double E,double alpha,double beta,double E0){
    //correction factor due to Scheinfein and Isaacson
    const double alphasq=pow(alpha,2.0);
    const double betasq=pow(beta,2.0);
    const double thetaE=E/(E0*(1022.0e3+E0)/(511.0e3+E0));
    const double thetaEsq=pow(thetaE,2.0);
    double corrfactor=1.0;
    const double eps=1e-6; //1 microrad is smallest reasonable convergence angle
    if ((thetaEsq!=0.0)&&(alpha>eps)){
         corrfactor=( sqrt(pow(alphasq+betasq+thetaEsq,2.0)-4.0*alphasq*betasq)-alphasq-betasq-thetaEsq+2.0*betasq*log((0.5/thetaEsq)*(sqrt(pow(alphasq+thetaEsq-betasq,2.0)+4.0*betasq*thetaEsq)+alphasq+thetaEsq-betasq))+2.0*alphasq*log((0.5/thetaEsq)*(sqrt(pow(betasq+thetaEsq-alphasq,2.0)+4.0*alphasq*thetaEsq)+betasq+thetaEsq-alphasq))    ) * 1.0/(2.0*alphasq*log(1.0+betasq/thetaEsq));
    }
    #ifdef COMPONENT_DEBUG
        std::cout <<"thetaE="<<thetaE<< "alpha="<<alpha<<"beta="<<beta<<"corrfactor="<<corrfactor<<"\n";
      #endif

    return corrfactor;
}

double Ledge::getcorrfactorKohl(double alpha,double beta,double theta){
    //described in H. Kohl Ultramicroscopy 19 (1985) 265-268
    //this factor can be inserted in the integration over theta to take into account the effect of convergence angle
    const double alphasq=pow(alpha,2.0);
    const double betasq=pow(beta,2.0);
    const double thetasq=pow(theta,2.0);
    double corrfactor=1.0;
    const double eps=1e-6; //1 microrad is smallest reasonable convergence angle
    const double Pi=acos(-1.0);

    if (alphasq<pow(eps,2.0)){
            //for very small alpha we need no correction factor, this avoids numerical trouble
            corrfactor=1.0;
            return corrfactor;
    }
    if (fabs(alpha-beta)>=theta){
        double thetapsq=alphasq;
        if (alphasq>betasq) {
            thetapsq=betasq;
        }
        corrfactor=thetapsq/alphasq;
        }
    else{
        //for abs(alpha-beta)<theta<alpha+beta (Make sure in the integration loop to go up to maximum alpha+beta
        const double x=(alphasq+thetasq-betasq)/(2.0*alpha*theta);
        const double y=(betasq+thetasq-alphasq)/(2.0*beta*theta);
        corrfactor=(1.0/Pi)*(acos(x)+(betasq/alphasq)*acos(y)-(1.0/(2.0*alphasq))*sqrt(4.0*alphasq*betasq-pow(alphasq+betasq-thetasq,2.0)));
        }
    #ifdef COMPONENT_DEBUG
        std::cout << " alpha="<<alpha<<" beta="<<beta<<" theta="<<theta << " corrfactor="<<corrfactor<<"\n";
    #endif
    return corrfactor;
}

void Ledge::setoptions(){
    estimateparams();
}
void Ledge::estimateparams(){ //estimate strength automatically
    //estimate strength automatically

    //calc integral in first 50eV
    Parameter* strengthptr= getparameter(5);
    const double strength=strengthptr->getvalue();
    const Parameter* Ekptr= getparameter(1);
    const double Ek=Ekptr->getvalue();
    const Model* mymodel=geteelsmodelptr()->getmodel();
    double area=0.0;
    double areaspec=0.0;
    for (unsigned int i=0;i<(this->getnpoints());i++){
        const double E=this->getenergy(i);
        if ((E>(Ek+10.0))&&(E<(Ek+50.0))){ //40eV integration window
            area+=this->getcounts(i);
            areaspec+=mymodel->getHLptr()->getcounts(i);
            //subtract all background components
            for (size_t j=0;j<mymodel->getcomponentsnr();j++){
                const Component* mycomponent=mymodel->getcomponent(j);
                if (mycomponent->getcanconvolute()==false){
                        areaspec-=mycomponent->getcounts(i);
                    }
            }
        }
    }

    //cal integral in same area in the spectrum without background
    const double newstrength=strength*(areaspec/area);
    strengthptr->setvalue(newstrength);
    //adjust strength
}

/* As it was in the old C eelsmodel
int sigmal2(double dsdE[],double Estart,double Estep,int channels,double E0,int Z,double edge,double thetastep,double thetamax)
     //sigmal 2 cross section
//L shell differentential cross section calcultation
//Calculate scattering cross sections via Generalised Oscillator Strength GOS
//Bethe Born theory
//Hydrogenic approximation of atoms
//ds/dE=Lxsection(Estart,Estop,Estep,E0,Ek,Z,thetastep,thetamax)
//Watch out all energies in eV
//all theta in rad
{
  double E;
  int i;
  double thetaE;
  double qa0sqmin;
  double qa0sq;
  double qa0;
  double Q;
  double khsq;
  double kh;
  double akh;
  double dfkdE;
  double y;
  double  eps=1.0e-99;
  double beta;
  double integral;
  double RF;
  double G;
  double C;
  double D;
  double A;

//Constants by Egerton
  double  XU[]={0.44,0.39,0.28,0.25,0.22,0.19,0.16,0.16,0.12,0.13,0.13,0.12,0.12,0.11,0.10,0.20,0.10,0.20};
  double  IE3[] = {73.0,99.0,135.0,164.0,200.0,245.0,294.0,347.0,402.0,455.0,513.0,575.0,641.0,710.0,779.0,855.0,931.0,1021.0,1115.0,1217.0,1323.0,1436.0,1550.0,1675.0};
  double  IE1[] = {118.0,149.0,189.0,229.0,270.0,320.0,377.0,438.0,500.0,564.0,628.0,695.0,769.0,846.0,926.0,1008.0,1096.0,1194.0,1142.0,1248.0,1359.0,1476.0,1596.0,1727.0};

  int  index=Z-13;
  double  U;
  double  EL3;
  double  EL1;
  double  Zs;

  //definition of fundamental constants
  double R=13.61; //Rydberg of energy in eV
  double e=1.602e-19; //electron charge in C
  double m0=9.110e-31; //electron rest mass in kg
  double a0=5.292e-11; //Bohr radius in m
  double c=2.998e8; //speed of light in m/s
  double hbar=1.055e-34; //Plank constant in J.s
  double lnqa0sq;
  double lnqa0sqstep;
  double qa0sqmax;
  //calculate relativistic correction factors
  double gamma=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
  double T=E0*(1.0+gamma)/(2.0*pow(gamma,2.0)); //in eV
  double vsq=2.0*e*T/m0; //speed of electrons in field

  U = XU[index];
  EL3 = IE3[index];
  EL1 = edge+IE1[index]-EL3; //take relative distance from start

  if (edge !=0.0) EL3=edge;
  //Reduce atomic number to account for electronic screening
  Zs=(double)Z-0.35*7.0-1.7;

  //Do energy and angle variations
  for (i=0;i<channels;i++)
    // Energy loop
    {
      E=Estart+(double)i*Estep;
      if (E>0.0)
	{
	  integral=0.0;
	  if (E>=EL3)
	    {
	      qa0sqmin=pow(E,2.0)/(4.0*R*T)+pow(E,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
	      qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin(thetamax/2.0),2.0);
	      lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/(thetamax/thetastep);

	      khsq=(E/(pow(Zs,2.0)*R))-0.25;
	      akh=sqrt(fabs(khsq));
	      RF=pow(((E+0.1-Ek)/(1.8*pow((double)Z,2.0))),U); //Correction factor for screening

	      //take linear steps in ln(qa0sq) grid
	      for (lnqa0sq=log(qa0sqmin);lnqa0sq<=log(qa0sqmax);lnqa0sq=lnqa0sq+lnqa0sqstep)
		//Angle loop
		{
		  qa0sq=exp(lnqa0sq);
		  Q=qa0sq/pow(Zs,2.0);
		  //Calculate GOS
		  if (khsq>=0.0)
		    {
		      beta=atan(akh/(Q-khsq+0.25));
		      if (beta<0.0) beta=beta+Pi;
		      C=exp(-2.0*beta/akh);
		      D=1.0-exp(-2.0*Pi/akh);
		    }
		  else
		    {
		      y=(-1.0/akh)*log((Q+0.25-khsq+akh)/(Q+0.25-khsq-akh));
		      C=exp(y);
		      D=1.0;
		    }

		  if (E<=EL1)
		    {
		      G=2.25*pow(Q,4.0)
			-(0.75+3.0*khsq)*pow(Q,3.0)
			  +(0.59375-0.75*khsq-0.5*pow(khsq,2.0))*pow(Q,2.0)
			    +(0.11146+0.85417*khsq+1.8833*pow(khsq,2.0)+pow(khsq,3.0))*Q
			      +0.0035807+khsq/21.333+pow(khsq,2.0)/4.5714+pow(khsq,3.0)/2.4+pow(khsq,4.0)/4.0;
		      A=pow((pow((Q-khsq+0.25),2.0)+khsq),5.0);
		      dfkdE=RF*32.0*G*C*E/(A*D*R*pow(Zs,4.0));
		    }
		  else
		    {
		      G=pow(Q,3.0)-((5.0/3.0)*khsq+11.0/12.0)*pow(Q,2.0)+((pow(khsq,2.0))/3.0+1.5*khsq+65.0/48.0)*Q+(pow(khsq,3.0))/3.0+0.75*pow(khsq,2.0)+(23.0/48.0)*khsq+5.0/64.0;
		      A=pow((pow(Q-khsq+0.25,2.0)+khsq),4.0);
		      dfkdE=RF*32.0*G*C*E/(A*D*R*pow(Zs,4.0));
		    }


		  //integration over theta
		  integral+=dfkdE*lnqa0sqstep;
		}
	    }
	  dsdE[i]= 4.0*Pi*pow(a0,2.0)*(R/(E*T))*integral*Estep; // in m**2/eV
	}
      else
	{
	  dsdE[i]=0.0;
	}
    }
}
*/


