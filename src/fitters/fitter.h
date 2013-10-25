/***************************************************************************
                          fitter.h  -  description
                             -------------------
    begin                : Sat Nov 16 2002
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

#ifndef FITTER_H
#define FITTER_H

//#define FITTER_DEBUG //print some debug info

#include <map>
#include <valarray>
#include <vector>

#include "src/core/curvematrix.h"
#include "src/core/parameter.h"
#include "src/core/model.h"
#include "src/core/chisquare.h"

/* A pure virtual function to derive different fitting classes. Takes a Model class and changes its free parameters to optimise the goodness of fit. */

class Fitter {
//member data
std::string fittertype;
std::string status; //a string containing status information of the fitter
bool candolin;

public:
Spectrum * tempspectrumptr; //a pointer to a spectrum for temporary storage

CurveMatrix* derivptr;
CurveMatrix* alphaptr;
CurveMatrix* modcurveptr; //modified curvature matrix
CurveMatrix* information_matrix; //the Fischer information matrix
std::valarray<double> beta;
//Spectrum* HLptr; //store it in the model, is no property of the fitter
Spectrum * residualspec;
Multispectrum * mresidualspec;

Model* modelptr;
CurveMatrix* curvaturematrixptr;
double fraction; //fraction of parametervalue used for numerical derivative
double eps; //some small number to avoid divide by zero
double minstep;
double maxstep;
double tolerance;
int nmax; //maximum number of itterations

bool usegradients;
bool dolin;
bool residual;
bool domulti;

bool constructed; //flag to determine wheter we are in the constructor or not
std::vector<double> goodnessqueue;
public:
  Fitter(Model*);
  std::string gettype(){return fittertype;}
  void settype(std::string type){fittertype=type;}
virtual ~Fitter();
virtual  double iteration();//do 1 iteration and return new goodness of fit
virtual  void iterate(int n);//do n iterations
virtual  bool iterate_and_check(int n);//do n iterations but stop earlier if converged
virtual double goodness_of_fit()const=0; //returns the goodness of fit
virtual std::string goodness_of_fit_string()const=0; //returns a string which says how good the fit is
double getminstep()const{return minstep;}
double getmaxstep()const{return maxstep;}
double getfraction()const{return fraction;}
int getnmax()const{return nmax;}   //max number of itterations
bool getusegradients()const{return usegradients;}
bool getdoresidual()const{return residual;}
void setminstep(double x){minstep=x;}
void setmaxstep(double x){maxstep=x;}
void setfraction(double x){fraction=x;}
void setusegradients(bool b){usegradients=b;}
void setdoresidual(bool b){residual=b;}
void settolerance(double t){tolerance=t;}
void setnmax(int n){nmax=n;}
double gettolerance()const{return tolerance;}
virtual void createmodelinfo(); //create all links to parameters of a model, do this whenever the model has changed
virtual void CRLB()=0; //calculate the sigma of the parameters by means of Cramer Rao lower bound
virtual double likelihoodratio()=0; //calculate the likelihood ratio
bool converged()const;//true if the change in goodness of fit was twice lower than tolerance
void addgoodness(double g); //update the goodness value queue
bool getmulti(){return modelptr->ismulti();} //returns wether the fitter is using a multispectrum or not
void setdomulti(bool b){domulti=b;} //define wether a fit should work on 1 or on all spectra in a multispectrum
bool getdomulti(){return domulti;}
int getnumberofspectra();
void setcurrentspectrum(unsigned int index);
Model* getmodelptr(){return modelptr;}
void setmodelptr(Model* m);
double degreesoffreedom()const;
double LRtestconfidence(); //return the confidence value for the LR test
std::string  LRtestconfidence_string();
virtual void updatemonitors(); //update the covariances for each parameter that has a monitor
virtual void calculate_derivmatrix(); //calculate alpha matrix
void updateresidual(); //a function that creates or updates the residual=model-experiment spectrum and shows it
virtual void initfitter();
void updateparam(Parameter* p,double newval);
void setstatus(std::string s);
std::string getstatus();
void dolintrick(bool b);
bool  getdolintrick()const;
bool candolintrick(){return candolin;}
void setcandolin(bool b){candolin=b;}

private:
virtual void partial_derivative(size_t j,const Spectrum* currentspectrum);  //partial derivative to a parameter Pj
                                                                            //(j index in freeparamvector)
                                                                            //wrt calculated model in currentspectrum
                                                                            //(to speed up calculation)
                                                                        //result is stored in derivptr matrix
virtual void calculate_beta_and_alpha();
virtual void modifiedcurve(double flambda);//calculate modified curvature matrix
virtual void newparameters(); //create new values for the parameters
virtual double estimatesigma(int j)const;//get estimate of sigma uncertainty on the ,new parameter j
virtual double getCRLB(int j){return 0.0;} //get Cramer Rao lower bound for the current parameters
void calculate_sigma()const;//calculate the standard deviation on the experimental data points and store them in the error bars of the spectrum
virtual void created_modelinfo(); //a function that is called at the end of createmodelinfo(), user can supply extra tasks to do when recreating model info

};

#endif
