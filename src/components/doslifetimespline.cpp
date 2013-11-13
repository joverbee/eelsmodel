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
 * eelsmodel components/doslifetimespline.cpp
 **/

//#define COMPONENT_DEBUG
#define SPLINE //else linear interpolation

#include "src/components/doslifetimespline.h"

#include <cmath>
#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <gsl/gsl_errno.h>
#include <gsl/gsl_spline.h>
#include <gsl/gsl_vector.h>

#ifdef DEBUG
    #include "debug_new.h" //memory leak checker
#endif

#include "src/components/gaussian.h"
#include "src/components/lorentz.h"

#include "src/core/parameter.h"

#include "src/gui/eelsmodel.h"
#include "src/gui/integerinput.h"
#include "src/gui/graph.h"
#include "src/gui/saysomething.h"
#include "src/gui/componentselector.h"

class QWorkspace;

QWorkspace* getworkspaceptr();
extern EELSModel* geteelsmodelptr();

DosLifetimeSpline::DosLifetimeSpline() //create a dummy version
:Component(),Evector(),Yvector(),b(),c(),d()
{
  dummy=0;
  compptr=0;
  name = "Fine Structure (DOS) with lifetime (cubic  spline)";
  this->setdescription("Fine Structure used in combination with a normal cross-section using Lifetime broadening as an extra prior knowledge");
  degree=0;
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);
  acc=0;
  sp=0;
  Plotspec=0;
}

DosLifetimeSpline::DosLifetimeSpline(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion),Evector(),Yvector(),b(),c(),d()
{
    broadeningtype=QUADRATIC;

    //offset=2;
    offset=2;
    //create spectrum with same energy scale as this
    dummy=new Spectrum(n,estart,dispersion);
Plotspec=0;
  compptr=0;
  if (parameterlistptr==0){
    //ask for the degree of the polynomial between 1 and 100
    const int min=1;
    const int max=1024;
    int d=10;
    Integerinput myinput(0,"","enter number of points",d,min,max);
    degree=size_t(d);
    //enter Estart and Estop
    Parameter* p1=new Parameter("Estart",estart,1);
    p1->interactivevalue("Enter Estart");
    p1->setchangeable(false);
    this->addparameter(p1);



    Parameter* p2=new Parameter("Estop",estart,1);
    p2->interactivevalue("Enter Estop");
    p2->setchangeable(false);
    this->addparameter(p2);


    //atomic to calculate the lifetime broadening
    Parameter* p3=new Parameter("a",0.4,1);

    //choose between different types of broadening
    int broadtype=0;
    Integerinput* myask=new Integerinput(0, "Lifetime broadening selector","1=Lin.,2=Quadr.,3=Egert.",broadtype,1,3);
    (void) myask;
    switch(broadtype){
        case 1:
            p3->setname("Linear coefficient");

            break;
        case 2:
            p3->setname("Quadratic coefficient");
            break;
        case 3:
            p3->setname("Egerton Broadening atomic distance [nm]");
            break;
        default:
            break;
    }
    p3->setboundaries(0,10.0);
    p3->interactivevalue("Enter broadening coefficient (0.0 for linear scale)");
    p3->setchangeable(false);
    this->addparameter(p3);

    //do we want linear or lifetime optimised energy points
    linear=false;
    if ((p3->getvalue())==0.0){
        //only for linear scale we need Estop
        //for lieftime broadening Estop is calculated by initenergy
        linear=true;
    }







    for (size_t i=0;i<degree;i++){
      std::string name;
      std::ostringstream s;
      if ((s << "a"<< i)){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     name=s.str();
      }
      //store parameters to hold the strengths of the basis set L
      Parameter* p=new Parameter(name,1.0,1);
      p->setboundaries(-1.0,10.0);
      //copy this parameter
      this->addparameter(p);
    }

    //now link this to a cross section which needs to be multiplied
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    int cnr=0;

    //create a componentselector here
    Componentselector myinput2(0,"","Select the component you want to multiply with",cnr);



    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
    set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
    if (compptr!=0){
      compptr->setmultiplierptr(this);
    }
    else{
      //something went wrong
      Saysomething mysay(0,"Error","the component didn't appear to be valid");
      throw Componenterr::unable_to_create();
    }
    //save this number in a parameter
    Parameter* p5=new Parameter("compnr",cnr,1);
    p5->setchangeable(false);
    this->addparameter(p5);
  }
  else{
    //get parameters from a list
    for (size_t i=0;i<(parameterlistptr->size());i++){
      Parameter* p=(*parameterlistptr)[i];
      this->addparameter(p);
    }
    degree=(parameterlistptr->size())-4;//there are 4 other parameters

    //tell the component that we multiply that we are here
    Parameter* p5=(*parameterlistptr)[parameterlistptr->size()-1];
    Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
    const int cnr=int(p5->getvalue());
    //get a pointer to this component and tell it that we are his multiplier
    compptr=mymodel->getcomponent(cnr);
    set_ismultiplier(true);//important do it here, otherwise multiplierptr is not accepted
    if (compptr!=0) {
      compptr->setmultiplierptr(this);
    }
#ifdef COMPONENT_DEBUG
      std::cout <<"After linking to the cross section\n";
#endif

} //end of else

  #ifdef COMPONENT_DEBUG
      std::cout <<"Setting the names etc\n";
#endif
  //give a name and description
  name = "Fine Structure (DOS) with lifetime (cubic  spline)";
  setdescription("Fine Structure used in combination with a normal cross-section using Lifetime broadening as an extra prior knowledge");
  setcanconvolute(true);
  setshifter(false);
  set_ismultiplier(true);


  /*
  for (size_t i=0;i<degree;i++){
    //tell that we have a gradient for each point a_i
    this->sethasgradient(i+3,true);
      }
*/


    InitEnergy(); //prepare the energy points that are linked to the paramters
    gslinit(); //setup the memory for the gsl fitter
    calculate();
    setvisible(true);



    Plotspec=new Spectrum(Evector.size());
    initplotspec();
    (this->getgraphptr())->addgraph(Plotspec);
    (this->getgraphptr())->setstyle(1,2); //set style of this plot to dots instead of lines


    //show the current DOS



    //make a new spectrum containing Evector,Yvector


    //show an equaliser
    this->showequalizer();
}
DosLifetimeSpline::~DosLifetimeSpline(){
  //killing the spectrum automatically cleans up the graph
  //tell the compptr that we are no longer multiplying it

  //check if the compptr is still valid, maybe it was killed and that is the reason why
  //we are getting deleted too
  try{
      const bool db=compptr->get_ismultiplier();
      (void) db;
      }
  catch(...){
      compptr=0;
  }

  if (compptr!=0){
    compptr->releasemultiplier();
  }
    //clean up the gsl stuff
    if (sp!=0) gsl_spline_free (sp);
    if (acc!=0) gsl_interp_accel_free (acc);

    if (Plotspec!=0) delete(Plotspec);

}

bool DosLifetimeSpline::checkxsection(){
  //check if the cross section component we are pointing to has still the same number
  //otherwise update the component number parameter
  Model* mymodel=geteelsmodelptr()->getmodel_nonconst();
  const int cnr=mymodel->getcomponentindexbypointer(compptr);
  if (compptr==NULL){
    return false;
  }
  Parameter* cnrptr= getparameter(degree+3);
  if ((cnr==-1)&&(compptr!=NULL)){
    //component not found also kill ourselves...
    Saysomething mysay(0,"Error","DOS: the component we are pointing to seems to be not present");

    delete(this);
    //it's over
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


void DosLifetimeSpline::calculate()
{
  if (!checkxsection()) return; //if a problem with the cross section we are pointing to, stop calculating
  //get the parameters
  const Parameter* Estartptr= getparameter(0);
  const double Estart=Estartptr->getvalue();
  const Parameter* Estopptr= getparameter(1);
  const double Estop=Estopptr->getvalue();
   const Parameter* aptr= getparameter(2);


  //calculate the DOS
  Parameter* pointptr=0; //pointer to a datapoint in the list
  bool changes=false;
  for (size_t index=0;index<degree;index++){
    pointptr=getparameter(index+3);
    changes=(changes||pointptr->changed()); //see if any of the data point parameters has changed
   }
  if ((Estartptr->changed()||Estopptr->changed())||aptr->changed()){
      //setup a new energy grid
    InitEnergy();
    changes=true;
  }
  if (changes){
    //if anything changed, we need to calculate, if not we leave without calculating
    #ifdef COMPONENT_DEBUG
    std::cout << "parameters changed calculating DOS \n degree: " << degree <<"\n";
    std::cout << "Estart: " << Estart <<"\n";
    std::cout << "Estop: " << Estop <<"\n";
    for (size_t index=0;index<degree;index++){
      std::cout<< " a" <<index<<" :"<<getparameter(index+3)->getvalue()<< "\n";
    }
    #endif


   //copy the parameter values in the spline vectors
    copyparameters();

    //calculate the spline parameters
    dospline();


    //do the evaluation
  for (size_t i=0; i<this->getnpoints();i++){
        const double en=this->getenergy(i);

        double cts=0.0;

        if ((en>Estart)&(en<Estop)){
          cts=seval(en);
        }
        if (en<=Estart){
            cts=0.0;
        }
        if (en>=Estop){
            cts=1.0;
        }
    this->setcounts(i,cts);
  }

  //set parameters as unchanged since last time we calculated
  this->setunchanged();

  }
  else{
#ifdef COMPONENT_DEBUG
    std::cout <<"parameters have not changed, i don't need to calculate again\n";
#endif
  }

}




Spectrum* DosLifetimeSpline::getgradient(size_t j){
    throw Componenterr::bad_index();
    return 0;
}

double  DosLifetimeSpline::Lifetimebroadening(double E){
  //calculate Lifetime broadening in eV according to Egerton 2007
  //E is the energy in eV above the onset

    const double dEmin=this->getdispersion(); //the minimum energy step is the dispersion
    const double dEmax=100.0; //maximum lifetime broadening, more doesn't make sense
    const double epsilon=E;
    const double h=4.13566733e-15; //eV/s
    const double m0=9.10938188e-31; //electron mass in kg
    const double e=1.60217646e-19; //electron charge (C)
    const double m=m0;
    const Parameter* aptr=getparameter(2);
    const double a=aptr->getvalue(); //atomic diameter in nm
    double lambda=0.0;
    double v=0.1;

    double tau=0.0;
    const double pi=acos(-1.0);
    const Parameter* Estartptr= getparameter(0);
    const double Estart=Estartptr->getvalue();
    const Parameter* Estopptr= getparameter(1);
    const double Estop=Estopptr->getvalue();
    //#ifdef COMPONENT_DEBUG
    //  std::cout <<"Z="<<Z<<" rho="<<rho<<" \n";
    //#endif
    broadeningtype=CONSTANT;
    if (aptr->getname()=="Linear coefficient") broadeningtype=LINEAR;
    if (aptr->getname()=="Quadratic coefficient") broadeningtype=QUADRATIC;
    if (aptr->getname()=="Egerton Broadening atomic distance [nm]") broadeningtype=EGERTON;
    if (a==0.0) broadeningtype=CONSTANT;
    double dE;

switch(broadeningtype)
{
    case EGERTON:
        //Egerton broadening
        if (epsilon>this->getdispersion()){
            v=sqrt(2.0*e*epsilon/m); //speed
        }
        lambda=538.0*fabs(a)*pow(fabs(epsilon),-2.0)+0.41*pow(fabs(a),3.0/2.0)*sqrt(fabs(epsilon));
        tau=lambda*1e-9/v; //lifetime in seconds
        //#ifdef COMPONENT_DEBUG
        //std::cout <<"v="<<v<<" tau="<<tau<<" lambda"<<lambda<<" a="<<a<<" \n";
        //#endif

        dE=h/(2.0*pi*tau);
        //if dE lower than dispersion or E< Eonset
       break;
    case QUADRATIC:
        //Quadratic broadening
        dE=fabs(a)*pow(fabs(epsilon),2.0); // a simple quadratic broadening
        //check for infinity
        break;
    case LINEAR:
        //broadening linear with energy above onset
         dE=fabs(a)*fabs(epsilon); // a simple linear broadening
         break;
    case CONSTANT:
    default:
        dE=(Estart-Estop)/degree;
        break;
}

if (dE<dEmin){
    dE=dEmin;
}
if (E<=0){
    dE=dEmin;
}
if (dE>dEmax){
    dE=dEmax;
}

//#ifdef COMPONENT_DEBUG
//std::cout <<"Returning a lifetime broadening of dE="<<dE<<"for energy E="<<E<<"\n";
//#endif
return dE;
}

void DosLifetimeSpline::Warp(Spectrum* L){
#ifdef COMPONENT_DEBUG
    std::cout <<"Entering Warping loop\n";
#endif
  //scale the original energy of the Lorentz component to 0 at onset
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  double Ewarp=0.0;
  for (size_t bin=0;bin<this->getnpoints();bin++){
    const double E=this->getenergy(bin);
    const double dE=Lifetimebroadening(E-Estart);
    Ewarp=Ewarp+1.0/dE;
    L->setenergy(bin,Ewarp);
  }
  //now shift this scale to have 0 at onset and have the same value at the highest index
  size_t Eminid=0;
  for (size_t bin=0;bin<this->getnpoints();bin++){
    if (this->getenergy(bin)<=Estart){
      Eminid=bin;
    }
  }
#ifdef COMPONENT_DEBUG
  std::cout <<"Eminid="<<Eminid<<"\n";
#endif
  const double Eonset=L->getenergy(Eminid);
  const double Emax=this->getenergy(this->getnpoints()-1)-this->getenergy(Eminid);
  const double Ewarpmax=L->getenergy(this->getnpoints()-1);
#ifdef COMPONENT_DEBUG
  std::cout <<"Eonset="<<Eonset<<" Emax="<<Emax<<" Ewarpmax="<<Ewarpmax<<"\n";
#endif

  for (size_t bin=0;bin<this->getnpoints();bin++){
    double E=L->getenergy(bin)-Eonset;
    E=E*Emax/Ewarpmax;
    L->setenergy(bin,E);
  }
#ifdef COMPONENT_DEBUG
    std::cout <<"End of  Warping loop\n";
#endif
  return;
}

void DosLifetimeSpline::InitEnergy(){


#ifdef COMPONENT_DEBUG
    std::cout <<"Initing Energy points\n";
#endif
  //do this at the start and whenever the Estart or Estop changes
  size_t Eminid=0;
  size_t Emaxid=0;
  const Parameter* Estartptr=this->getparameter(0);
  const double Estart=Estartptr->getvalue();
  Parameter* Estopptr= this->getparameter(1);
  double Estop=Estopptr->getvalue();

  for (size_t bin=0;bin<this->getnpoints();bin++){
    if (this->getenergy(bin)<=Estart){
      Eminid=bin;
    }
    if (this->getenergy(bin)<=Estop){
      Emaxid=bin;
    }
  }

#ifdef COMPONENT_DEBUG
  std::cout <<"Eminid: "<<Eminid<<"\n";
  std::cout <<"Emaxid: "<<Emaxid<<"\n";
#endif




    // copy parameters in Yvector and initialise
    Yvector.clear();
    Evector.clear();

    //add a first point that is zero and has energy Estart
    //if offset=2 add another point to make sure the aproach is flat
    if (offset==2){
        Yvector.push_back(0.0);
        Evector.push_back(Estart-1.0);
    }
    Yvector.push_back(0.0);
    Evector.push_back(Estart);


    //add points defined by the parameters


    //determine where we end up in term of E when doing the Energy sum
    double Eposp=Estart;
    for (size_t index=0;index<=degree;index++){//do one more to end up on the point that should be Estop
        Eposp=Eposp+Lifetimebroadening(Eposp-Estart);
    }
    //Eposp is now the energy where we would end if taking the broadening
    //we want the energy to end at Estop
    //so we rescale with
    const double scale=(Eposp-Estart)/(Estop-Estart);

    double Epos=Estart;
    for (size_t index=0;index<degree;index++){
        //copy the parameter values in a vector
        Yvector.push_back((this->getparameter(index+3))->getvalue());
        Epos=Epos+Lifetimebroadening(Epos-Estart);
           // Warp(dummy);
            //Epos=this->getenergy(Eminid)+(double(index+offset)/(double(degree+offset)))*(dummy->getenergy(Emaxid)-dummy->getenergy(Eminid));

        Evector.push_back(Estart+(Epos-Estart)/scale);

    }

    //recalculate Estop

    //add a last point that is 1 at Estart
    Yvector.push_back(1.0);
    Evector.push_back(Estop);
    //a second last point to make sure the end of the spline aproache the tail with a flat section
    if (offset==2){
        Yvector.push_back(1.0);
        Evector.push_back(Estop+1.0);
    }

#ifdef COMPONENT_DEBUG
std::cout <<"Evector\n";
for (size_t i=0;i<Evector.size();i++){
  std::cout <<"Evector[ "<<i<<"]="<<Evector[i]<<"\n";
}
#endif

}

void DosLifetimeSpline::copyparameters(){
    //copy the paramters values in the Yvector
    for (size_t index=0;index<degree;index++){
        //copy the parameter values in a vector, remember that point Yvector[0] is the first point but not connected to the parameters
        Yvector[index+offset]=((this->getparameter(index+3))->getvalue());
    }
}



DosLifetimeSpline* DosLifetimeSpline::clone()const{
  return new DosLifetimeSpline(*this);
}
DosLifetimeSpline* DosLifetimeSpline::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
  return new DosLifetimeSpline(n,estart,dispersion,parameterlistptr);
}

void DosLifetimeSpline::dospline(){
    //calculate the spline coefficients
     gsl_spline_init (sp, &Evector[0], &Yvector[0], Evector.size());



}

double DosLifetimeSpline::seval(double x){
    return gsl_spline_eval (sp, x, acc);
}


void DosLifetimeSpline::gslinit(){
    //init a cubic spline
    acc= gsl_interp_accel_alloc ();
    sp= gsl_spline_alloc (gsl_interp_cspline, Evector.size());
}
void DosLifetimeSpline::initplotspec(){
    //copy Yvector and Evector in plotspec
    if (Plotspec!=0){
        for (size_t i=0;i<Plotspec->getnpoints();i++){
            Plotspec->setdatapoint(i,Evector[i],Yvector[i],0.0);
        }
        (this->getgraphptr())->updategraph(1,Plotspec);
    }
}

//inherit show but also redefine what should happen with the dos window
void DosLifetimeSpline::show(){
    Component::show(); //do the normal show
    initplotspec(); //but also update the points on the plot

    //copy energy values in the names of the parameters
    //this gives users the possiblity to know at what energy the dos points where taken
    for (size_t index=0;index<degree;index++){
        Parameter* pointptr=getparameter(index+3);
        std::string namestring;
        std::ostringstream s;
        if (s << "a" << index <<" @ " << Evector[index+offset] <<" eV"){ //converting an int to a string in c++ style rather than unsafe c-style
	     // conversion worked
	     namestring=s.str();
        }
        pointptr->setname(namestring);
   }
}

void DosLifetimeSpline::orderchanged(){
    //in case the order of the components changes, update the compnr
    const Model* mymodel=geteelsmodelptr()->getmodel();
    const int cnr=mymodel->getcomponentindexbypointer(compptr);
    Parameter* cnrptr= getparameter(degree+3);
    cnrptr->setchangeable(true); //unlock
    cnrptr->setvalue(cnr); //update to current value
    cnrptr->setchangeable(false); //lock
}
