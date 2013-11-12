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
 * eelsmodel - components/powerlaw.cpp
 **/

#include "src/components/powerlaw.h"

#include <cmath>
#include <iostream>

#include "src/core/eelsmodel.h"
#include "src/core/model.h"
#include "src/core/parameter.h"

#include "src/gui/graph.h"
#include "src/gui/saysomething.h"

Eelsmodel* geteelsmodelptr();
QWorkspace* getworkspaceptr();

Powerlaw::Powerlaw() //create a dummy version
:Component()
{
setname("Power law background");
setdescription("y=A(E/E0)^-r with A and r the two parameters");
setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
setshifter(false);

}
Powerlaw::Powerlaw(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{
  //add the required parameters
  Parameter* p1;
  Parameter* p2;
  if (parameterlistptr==0){
    p1=new Parameter("A",1.0e3,1);
    p2=new Parameter("r",3.0,1);
    p2->setboundaries(1.0,10.0);
  }
  else{
    p1=(*parameterlistptr)[0];
    p2=(*parameterlistptr)[1];
  }

  //parameter A is a linear parameter
  p1->setlinear(true);

  this->addparameter(p1);
  this->addparameter(p2);
  //give a name and description
  setname("Power law background");
  setdescription("y=A(E/E0)^-r with A and r the two parameters");
  setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
  setshifter(false);
  //an analytical gradient is available for both parameters
  sethasgradient(0,true);
  sethasgradient(1,true);


  if (parameterlistptr==0){
   //determine start params from fit in selected area if not created from a paramterlist
    estimateparams();
  }
}

Powerlaw::~Powerlaw(){
}
void Powerlaw::calculate()
{
//check if any of paramters is changed, if not : don't calculate
const Parameter* p1= getparameter(0);
const double A=p1->getvalue();
const Parameter* p2= getparameter(1);
const double r=p2->getvalue();

if (p1->changed()||p2->changed())
{
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating power law \n A: " << A << " r:" <<r<< "\n";
  #endif
  
  //const double en0=fabs(this->getenergy(0)); //avoid a negative E0, this only happens for pathological spectra
  //get the first non excluded energy
  const Model* mymodel=geteelsmodelptr()->getmodel();
  const double en0=fabs(mymodel->getfirstnonexludeenergy());
  
  for (size_t i=0;i<(this->getnpoints());i++){
    const double en=this->getenergy(i);
    if ((en>0.0)&&(r>0.0)){
      const double cts=A*pow((en0/en),r);
      this->setdatapoint(i,en,cts,0.0);
    }
    else{
      //power law is only meaningful for E>0 fill in zero for E<=0, and if r>0 to prevent pow domain error
      this->setdatapoint(i,en,0.0,0.0);
    }
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
Spectrum* Powerlaw::getgradient(size_t j){
//get analytical partial derivative to parameter j in point i
  const Parameter* Aptr= getparameter(0);
  double A=Aptr->getvalue();
  const Parameter* rptr= getparameter(1);
  double r=rptr->getvalue();
  //double en0=fabs(this->getenergy(0)); //avoid negative E0 this happens only for pathological spectra (a power law has no meaning for a neg energy)
  const Model* mymodel=geteelsmodelptr()->getmodel();
  const double en0=fabs(mymodel->getfirstnonexludeenergy());

  #ifdef COMPONENT_DEBUG
  std::cout << "calculating the partial derivative in A: " << A << " r:" <<r<<"\n";
  #endif
  switch(j){
  case 0:
  //analytical derivative wrt A
    for (size_t i=0;i<this->getnpoints();i++)
  {
      double en=this->getenergy(i);
      if ((en>0.0)&&(r>0.0)){
	gradient.setcounts(i,pow((en0/en),r));
      }
      else{
	//only meaningfull when E>0 and r>0
	gradient.setcounts(i,0.0);
      }
  }

  break;
  case 1:
  //analytical derivative wrt r
   for (unsigned int i=0;i<(this->getnpoints());i++)
  {
      double en=this->getenergy(i);
      if ((en>0.0)&&(r>0.0)){
	gradient.setcounts(i,A*pow((en0/en),r)*log(en0/en));
      }
      else{
	//only meaningfull when E>0 and r>0
	gradient.setcounts(i,0.0);
      }
  }

  break;
  default:
  throw Componenterr::bad_index();
  }
  return &gradient;
}

Powerlaw* Powerlaw::clone()const{
return new Powerlaw(*this);}
Powerlaw* Powerlaw::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new Powerlaw(n,estart,dispersion,parameterlistptr);
}

void Powerlaw::fitpowerlaw(double&A,double&r,size_t startindex,size_t stopindex){
     const Model* mymodel=geteelsmodelptr()->getmodel();
    //do a power law fit and determine parameters A and r
    if (startindex>=stopindex){
        //impossible to fit
        A=0.0;
        r=-3.0;
        return;
    }
   const double N=double(stopindex)-double(startindex);
    //see Egerton p271
    double xy=0.0;
    double sx=0.0;
    double sy=0.0;
    double sxsq=0.0;
    for (size_t i=startindex;i<stopindex;i++){
        const double x=log(this->getenergy(i));
        const double y=log((mymodel->getHLptr())->getcounts(i));
        xy+=x*y;
        sx+=x;
        sy+=y;
        sxsq+=pow(x,2.0);
    }
    const double b=(N*xy-sx*sy)/(N*sxsq-pow(sx,2.0));
    const double a=sy/N-b*sx/N;
    r=-b;
    A=exp(a);



    #ifdef COMPONENT_DEBUG
    double yavgfit=0.0;
    double yavg=0.0;
    for (size_t i=startindex;i<stopindex;i++){
        const double en=this->getenergy(i);
        const double fit=A*pow(en,-r);
        yavgfit+=fit;
        yavg+=(mymodel->getHLptr())->getcounts(i);
    }
    yavgfit=yavgfit/N;
    std::cout<<"power law fit A="<<A<<" r="<<r<<" yavg="<<yavg<<" yavgfit="<<yavgfit<<"\n";
    #endif

    //sanity check
    if (r<1.0){
        r=3.0;
        A=0.0;
    }
    if (A<0.0){
        r=3.0;
        A=0.0;
    }
return;
}

void Powerlaw::setoptions(){
    //if right button pressed...refit the params
    estimateparams();
}
void Powerlaw::estimateparams(){
    double A=0.0;
    double r=0.0;
    int startindex=0;
    int endindex=this->getnpoints()-1;
    const Model* mymodel=geteelsmodelptr()->getmodel();
    const Graph*  topgraph=mymodel->getgraphptr();
    Parameter* p1=this->getparameter(0);
    Parameter* p2=this->getparameter(1);

    if (topgraph!=0){
        if (topgraph->getselected()){//if something selected
            startindex=topgraph->getstartindex();
            endindex=topgraph->getendindex();
            fitpowerlaw(A,r,startindex,endindex);
            
            const double en0=mymodel->getfirstnonexludeenergy();
            A=A*pow(en0,-r); //correct to E0^-r scaling
            //check sanity else do interactive....
            //force these values in case of locked params, otherwise the fit has a strange effect
            const bool lockstate1=p1->ischangeable();
            p1->setchangeable(true);
            p1->setvalue(A);
            p1->setchangeable(lockstate1);


            const bool lockstate2=p2->ischangeable();
            p2->setchangeable(true);
            p2->setvalue(r);
            p2->setchangeable(lockstate2);
        }
    }
    else{
        //didn't work do manual entering of A and r
        Saysomething mysay(0,"Info","If you select a region on the model graph\n, A and r will be estimated from a power law fit \n when right clicking powerlaw",true);
    }
return;
}
