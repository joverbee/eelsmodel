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
 * eelsmodel - components/fast_BG.cpp
 **/

#include <cmath>
#include <iostream>

#include "src/components/fast_BG.h"
#include "src/components/powerlaw.h"

#include "src/core/eelsmodel.h"
#include "src/core/model.h"
#include "src/core/parameter.h"
#include "src/core/slice_iter.h"

#include "src/gui/graph.h"
#include "src/gui/saysomething.h"

Eelsmodel* geteelsmodelptr();
QMdiArea* getworkspaceptr();

fast_BG::fast_BG() //create a dummy version
:Component()
{
setname("Fast background");
setdescription("y=A1(E/E0)^-r1+A2(E/E0)^-r'' with A1,A2 the two parameters. r and r' are constant so this is a linear function (therefore fast)");
setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
setshifter(false);

}
fast_BG::fast_BG(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)
:Component(n,estart,dispersion)
{

    //create the parameters
    //add the required parameters
    Parameter* p1; //A1
    Parameter* p2; //A2,
    Parameter* p3; //r1
    Parameter* p4; //r2

    if (parameterlistptr==0){
      p1=new Parameter("A1",1.0e3,1);
      p2=new Parameter("A2",1.0e3,1);
      p3=new Parameter("r1",2.0,0);
      p4=new Parameter("r2",3.0,0);
      p1->setboundaries(-1.0e10,1.0e10);
      p2->setboundaries(-1.0e10,1.0e10);
      p3->setboundaries(1.0,10.0);
      p4->setboundaries(1.0,10.0);

    }
    else{
      p1=(*parameterlistptr)[0];
      p2=(*parameterlistptr)[1];
      p3=(*parameterlistptr)[2];
      p4=(*parameterlistptr)[3];
    }

    //parameter A1 and A2 are linear parameters
    p1->setlinear(true);
    p2->setlinear(true);


    this->addparameter(p1);
    this->addparameter(p2);
    this->addparameter(p3);
    this->addparameter(p4);

    //give a name and description
    setname("Fast background");
    setdescription("y=A1(E/E0)^-r1+A2(E/E0)^-r'' with A1,A2 the two parameters. r and r' are constant so this is a linear function (therefore fast)");
    setcanconvolute(false); //don't convolute the background it only gives problems and adds no extra physics
    setshifter(false);
    //an analytical gradient is available for all parameters
    sethasgradient(0,true);
    sethasgradient(1,true);
    sethasgradient(2,true);
    sethasgradient(3,true);


    if (parameterlistptr==0){
     //determine start params from fit in selected area if not created from a paramterlist
      estimateparams();
    }
}

fast_BG::~fast_BG(){


}
void fast_BG::calculate()
{
//check if any of paramters is changed, if not : don't calculate
const Parameter* p1= getparameter(0);
const double A1=p1->getvalue();
const Parameter* p2= getparameter(1);
const double A2=p2->getvalue();
const Parameter* p3= getparameter(2);
const double r1=p3->getvalue();
const Parameter* p4= getparameter(3);
const double r2=p4->getvalue();

if (p1->changed()||p2->changed()||p3->changed()||p4->changed())
{
  #ifdef COMPONENT_DEBUG
  std::cout << "parameters changed calculating fast_BG \n A1: " << A1 << " A2:" <<A2<< " r1: "<<r1<<" r2: "<<r2<<"\n";
  #endif

  const Model* mymodel=geteelsmodelptr()->getmodel();
  const double en0=fabs(mymodel->getfirstnonexludeenergy());
  for (size_t i=0;i<this->getnpoints();i++){
        double en=this->getenergy(i);
        const double cts=A1*pow((en0/en),r1)+A2*pow((en0/en),r2);//a sum of 2 power laws
        this->setcounts(i,cts);
  }
  this->setunchanged();
}
else{
  #ifdef COMPONENT_DEBUG
  std::cout <<"parameters have not changed, i don't need to calculate again\n";
  #endif
    }
}
Spectrum* fast_BG::getgradient(size_t j){
  //get analytical partial derivative to parameter j in point i
  const Parameter* A1ptr= getparameter(0);
  double A1=A1ptr->getvalue();
  const Parameter* A2ptr= getparameter(1);
  double A2=A2ptr->getvalue();
  const Parameter* r1ptr= getparameter(2);
  double r1=r1ptr->getvalue();
  const Parameter* r2ptr= getparameter(3);
  double r2=r2ptr->getvalue();



  //double en0=fabs(this->getenergy(0)); //avoid negative E0 this happens only for pathological spectra (a power law has no meaning for a neg energy)
  const Model* mymodel=geteelsmodelptr()->getmodel();
  const double en0=fabs(mymodel->getfirstnonexludeenergy());

  #ifdef COMPONENT_DEBUG
  std::cout << "calculating the partial derivative in A1: " << A1 << " r:" <<r<<"\n";
  #endif
  switch(j){
  case 0:
  //analytical derivative wrt A
    for (size_t i=0;i<this->getnpoints();i++)
  {
      double en=this->getenergy(i);
      if ((en>0.0)&&(r1>0.0)&&(r2>0.0)){
          gradient.setcounts(i,pow((en0/en),r1));
      }
      else{
	//only meaningfull when E>0 and r>0
	gradient.setcounts(i,0.0);
      }
  }

  break;
  case 1:
  //analytical derivative wrt A2
   for (unsigned int i=0;i<(this->getnpoints());i++)
  {
      double en=this->getenergy(i);
       if ((en>0.0)&&(r1>0.0)&&(r2>0.0)){
           gradient.setcounts(i,pow((en0/en),r2));
        }
      else{
	//only meaningfull when E>0 and r>0
	gradient.setcounts(i,0.0);
      }
  }

  break;

  case 2:
  //analytical derivative wrt r1
   for (unsigned int i=0;i<(this->getnpoints());i++)
  {
      double en=this->getenergy(i);
       if ((en>0.0)&&(r1>0.0)&&(r2>0.0)){
        gradient.setcounts(i,A1*pow((en0/en),r1)*log(en0/en));
      }
      else{
    //only meaningfull when E>0 and r>0
    gradient.setcounts(i,0.0);
      }
  }

  break;

case 3:
//analytical derivative wrt r2
 for (unsigned int i=0;i<(this->getnpoints());i++)
{
    double en=this->getenergy(i);
     if ((en>0.0)&&(r1>0.0)&&(r2>0.0)){
        gradient.setcounts(i,A2*pow((en0/en),r2)*log(en0/en));
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

fast_BG* fast_BG::clone()const{
return new fast_BG(*this);}
fast_BG* fast_BG::new_component(int n,double estart,double dispersion,std::vector<Parameter*>* parameterlistptr)const{
return new fast_BG(n,estart,dispersion,parameterlistptr);
}

void fast_BG::fitfast_BG(double&A,double&r,size_t startindex,size_t stopindex){
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

void fast_BG::setoptions(){
    //if right button pressed...refit the params
    estimateparams();
}
void fast_BG::estimateparams(){
    double A=0.0;
    double r=0.0;
    int startindex=0;
    int endindex=this->getnpoints()-1;
    const Model* mymodel=geteelsmodelptr()->getmodel();
    const Graph*  topgraph=mymodel->getgraphptr();

    Parameter* p1=this->getparameter(0);
    Parameter* p2=this->getparameter(1);
    Parameter* p3=this->getparameter(2);
    Parameter* p4=this->getparameter(3);

    if (topgraph!=0){
        if (topgraph->getselected()){//if something selected
            startindex=topgraph->getstartindex();
            endindex=topgraph->getendindex();
            fitfast_BG(A,r,startindex,endindex);
            
            const double en0=mymodel->getfirstnonexludeenergy();
            A=A*pow(en0,-r); //correct to E0^-r scaling
            //check sanity else do interactive....

            //choose r1 and r2 values around the range of r that we found
            double r1=2.0;
            double r2=3.0;

            if ((r>1.0)&&(r<10.0)){
                r1=std::floor(r);
                r2=std::ceil(r);
            }
            //determine x from that
            double x=(r-r1)/(r1-r2);
            if (x<0.0){
                x=0.0;
            }
            if (x>1.0){
                x=1.0;
            }

            //force these values in case of locked params, otherwise the fit has a strange effect
            const bool lockstate1=p1->ischangeable();
            p1->setchangeable(true);
            p1->setvalue(A*x);
            p1->setchangeable(lockstate1);


            const bool lockstate2=p2->ischangeable();
            p2->setchangeable(true);
            p2->setvalue(A*(1-x));
            p2->setchangeable(lockstate2);

            const bool lockstate3=p3->ischangeable();
            p3->setchangeable(true);
            p3->setvalue(r1);
            p3->setchangeable(lockstate3);

            const bool lockstate4=p4->ischangeable();
            p4->setchangeable(true);
            p4->setvalue(r2);
            p4->setchangeable(lockstate4);

        }
    }
    else{
        //didn't work do manual entering of A and r
        Saysomething mysay(0,"Info","If you select a region on the model graph\n, A and r will be estimated from a power law fit \n when right clicking fast_BG",true);
    }
return;
}
