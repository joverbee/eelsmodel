/***************************************************************************
                          kedge.cpp  -  description
Hydrogenic K-edge component based on SigmaK by R.F. Egerton
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

#include "src/components/kedge.h"
#include "src/core/parameter.h"
#include <cmath>
#include <iostream>

#include "src/core/eelsmodel.h"

extern Eelsmodel* geteelsmodelptr();//main.cpp contains this global function with a pointer to eelsmodel
//#define COMPONENT_DEBUG

Kedge::Kedge() //create a dummy version
:Component(),tempspectrum()
{
this->setname("Hydrogenic K-edge");
this->setdescription("Hydrogenic K-edge based on SigmaK by R.F. Egerton");
}
Kedge::Kedge(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
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

    p2=new Parameter("Ek",estart,1);
    p2->interactivevalue("enter edge energy [eV]");

    p3=new Parameter("Z",8.0,1);
    p3->interactivevalue("enter atomic number Z");
    p3->setchangeable(false);  //the fitter should not change this in normal operation

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
  this->setname("Hydrogenic K-edge");
  this->setdescription("Hydrogenic K-edge based on SigmaK by R.F. Egerton");
  this->setcanconvolute(true);
  //we have gradient wrt the strength param
  this->sethasgradient(5,true);
  setshifter(false);
}
Kedge::~Kedge(){
}
void Kedge::calculate(){
  //get the parameters
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


  //if parameters have changed, calculate again
  if (E0ptr->changed()||Ekptr->changed()||Zptr->changed()||thetastepptr->changed()||thetamaxptr->changed()||alphaptr->changed()){
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating K edge \n E0: " << E0 << " Ek:" <<Ek
            << " Z: "<<Z<<" thetasteps: "<<thetasteps<<" thetamax: "<<thetamax<<" strength: "<<strength<<"\n";
    #endif

    const double dispersion=this->getdispersion();
    const double eps=1.0e-99;
    //definition of fundamental constants
    const double R=13.606; //Rydberg of energy in eV
    const double e=1.602e-19; //electron charge in C
    const double m0=9.110e-31; //electron rest mass in kg
    const double a0=5.292e-11; //Bohr radius in m
    const double c=2.998e8; //speed of light in m/s
    const double Pi=acos(-1.0);
    //calculate relativistic correction factors
    const double gamma=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
    const double T=E0*(1.0+gamma)/(2.0*pow(gamma,2.0)); //in eV

    //Reduce atomic number to account for electronic screening
    const double Zs=Z-0.3125;
    double cts=0.0;
    for (unsigned int i=0;i<(this->getnpoints());i++){//energy loop
      const double E=this->getenergy(i);
      double integral=0.0;
      if ((E>=Ek)&&(E>0.0)){
        const double qa0sqmin=pow(E,2.0)/(4.0*R*T)+pow(E,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
        //change max angle to beta+alpha and do Kohl correction
        const double qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin((thetamax+alpha)/2.0),2.0);
        const double lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/floor(thetasteps);
        const double khsq=E/(pow(Zs,2.0)*R)-1.0;
        // do on a logarithmic grid lnqa0sq
        for (double lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq+=lnqa0sqstep){//collection angle loop
          const double qa0sq=exp(lnqa0sq);
          //Dimensionless variables
          const double Q=qa0sq/pow(Zs,2.0);
          //Calculate GOS
          double dfkdE=0.0;
          if (E<=(pow(Zs,2.0)*R)){
            const double im=sqrt(-khsq);
            const double y=-(1.0/im)*log((Q+1.0-khsq+2.0*im)/(Q+1.0-khsq-2.0*im+eps));
            dfkdE=256.0*E*(Q+khsq/3.0+1.0/3.0)*exp(y)/(pow(Zs,4.0)*pow(R,2.0)*pow((pow((Q-khsq+1.0),2.0)+4.0*khsq),3.0)+eps);
            }
          else{
            const double kh=sqrt(khsq);
            double beta=atan(2.0*kh/(Q-khsq+1.0));
            if (beta<0.0) beta=beta+Pi;
            dfkdE=256.0*E*(Q+khsq/3.0+1.0/3.0)*exp(-2.0*beta/kh)/(pow(Zs,4.0)*pow(R,2.0)*pow( ( pow((Q-khsq+1.0),2.0) +4.0*khsq),3.0)*(1.0-exp(-2.0*Pi/kh))+eps);
            }
            //integration over theta
            const double theta=2.0*sqrt(fabs(R*(qa0sq-qa0sqmin)/(4.0*pow(gamma,2.0)*T))); //fabs to avoid small neg number to cause probs with sqrt
            //see eq. 3.146 in Egerton
            integral+=getcorrfactorKohl(alpha,thetamax,theta)*dfkdE*lnqa0sqstep;
          }//end of angle loop
        cts= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E*T))*integral*dispersion; /* in m**2/eV */
        //try larger for better numerical behaviour
        //const double cts= (1.0/E)*integral*dispersion; /* not in m**2/eV */
        }//end of if E>=Ek
      else{
        cts=0.0;
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


Spectrum* Kedge::getgradient(size_t j){
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

Kedge* Kedge::clone()const{
return new Kedge(*this);}
Kedge* Kedge::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Kedge(n,estart,dispersion,parameterlistptr);
}


double Kedge::getcorrfactor(double E,double alpha,double beta,double E0){
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
        std::cout <<"E="<<E<<" thetaE="<<thetaE<< " alpha="<<alpha<<" beta="<<beta<<" corrfactor="<<corrfactor<<"\n";
      #endif

    return corrfactor;
}

double Kedge::getcorrfactorKohl(double alpha,double beta,double theta){
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
void Kedge::setoptions(){
    estimateparams();
}
void Kedge::estimateparams(){ //estimate strength automatically
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
            if (this->get_hasmultiplier()){
                area+=this->getcounts(i)*((this->getmultiplierptr())->getcounts(i));
            }
            else{
                area+=this->getcounts(i);
            }
            areaspec+=mymodel->getHLptr()->getcounts(i);

            //subtract all background components
            for (size_t j=0;j<mymodel->getcomponentsnr();j++){
                const Component* mycomponent=mymodel->getcomponent(j);
                //if (mycomponent->getcanconvolute()==false){
                  if ((mycomponent!=this)&&(mycomponent->get_ismultiplier()==false)&&(mycomponent->getconvolutor()==false)){
                      //subtract all expect myself and all other multipliers and convolutors
                        areaspec-=mycomponent->getcounts(i);
                    }
            }
        }
    }
    const double newstrength=strength*(areaspec/area);
    //adjust strength
    strengthptr->setvalue(newstrength);

}

/*  as it was in the old C EELSMODEL

int Kxsection(double dsdE[],double Estart,double Estep,int channels,double E0,double Ek,double Z,double thetastep,double thetamax)
     //K edge cross section SIGMAK

	//K shell differentential cross section calcultation
	//Calculate scattering cross sections via Generalised Oscillator Strength GOS
	//Bethe Born theory
	//Hydrogenic approximation of atoms
	//ds/dE=Kxsection(Estart,Estop,Estep,E0,Ek,Z,thetastep,thetamax)
	//Watch out all energies in eV
	//all theta in rad
{
  double E;
  int i;
  double theta;
  double thetaE;
  double qa0sqmin;
  double qa0sq;
  double qa0;
  double Q;
  double khsq;
  double kh;
  double dfkdE;
  double y;
  const eps=1.0e-99;
  double beta;
  double integral;
  double im;
  double lnqa0sq;
  double qa0sqmax;
  double lnqa0sqstep;
  //definition of fundamental constants
  double R=13.61; //Rydberg of energy in eV
  double e=1.602e-19; //electron charge in C
  double m0=9.110e-31; //electron rest mass in kg
  double a0=5.292e-11; //Bohr radius in m
  double c=2.998e8; //speed of light in m/s
  double hbar=1.055e-34; //Plank constant in J.s

  //calculate relativistic correction factors
  double gamma=1.0+(e*E0)/(m0*pow(c,2.0)); //dimensionless
  double T=E0*(1.0+gamma)/(2.0*pow(gamma,2.0)); //in eV
  double vsq=2.0*e*T/m0; //speed of electrons in field

  //Reduce atomic number to account for electronic screening
  double Zs=(double)Z-0.3125;

  //Do energy and angle variations

  for (i=0;i<channels;i++)
    // Energy loop
    {
      E=Estart+(double)i*Estep;
      if (E>0.0)
	{
	  integral=0;
	  if (E>=Ek)
	    {
	      thetaE=E/(2.0*gamma*T); //effective scattering angle
	      qa0sqmin=pow(E,2.0)/(4.0*R*T)+pow(E,3.0)/(8.0*pow(gamma,3.0)*R*pow(T,2.0)); //partial expansion for theta=0
	      qa0sqmax=qa0sqmin+4.0*pow(gamma,2.0)*(T/R)*pow(sin(thetamax/2.0),2.0);
	      lnqa0sqstep=(log(qa0sqmax)-log(qa0sqmin))/(thetamax/thetastep);

	      khsq=E/(pow(Zs,2.0)*R)-1.0;
	      // do on a logarithmic grid lnqa0sq
	      for (lnqa0sq=log(qa0sqmin);lnqa0sq<log(qa0sqmax);lnqa0sq=lnqa0sq+lnqa0sqstep)
		//Angle loop
		{
		  qa0sq=exp(lnqa0sq);
		  //Dimensionless variables
		  Q=qa0sq/pow(Zs,2.0);
		  //Calculate GOS
		  if (E<=(pow(Zs,2.0)*R))
		    {
		      im=sqrt(-khsq);
		      y=-(1.0/im)*log((Q+1.0-khsq+2.0*im)/(Q+1.0-khsq-2.0*im+eps));
		      dfkdE=256.0*E*(Q+khsq/3.0+1.0/3.0)*exp(y)/(pow(Zs,4.0)*pow(R,2.0)*pow((pow((Q-khsq+1.0),2.0)+4.0*khsq),3.0)+eps);
		    }
		  else
		    {
		      kh=sqrt(khsq);
		      beta=atan(2.0*kh/(Q-khsq+1.0));
		      if (beta<0.0) beta=beta+Pi;
		      dfkdE=256.0*E*(Q+khsq/3.0+1.0/3.0)*exp(-2.0*beta/kh)/(pow(Zs,4.0)*pow(R,2.0)*pow( ( pow((Q-khsq+1.0),2.0) +4.0*khsq),3.0)*(1.0-exp(-2.0*Pi/kh))+eps);
		    }


		  //integration over theta
		  integral+=dfkdE*lnqa0sqstep;
		}
	    }
	  dsdE[i]= 4.0*Pi*pow(a0,2.0)*(pow(R,2.0)/(E*T))*integral*Estep; // in m**2/eV
	}
      else
	{
	  dsdE[i]=0.0;
	}
    }
}
*/
