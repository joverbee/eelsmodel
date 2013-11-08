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
 * eelsmodel - fitters/fitter.cpp
 **/

#include "src/fitters/fitter.h"

//#define FITTER_DEBUG
#include <cmath>
#include <cstdio>
#include <iostream>
#include <limits>
#include <valarray>
#include <vector>

#include <Eigen/Dense>

#include "src/core/component.h"
#include "src/core/parameter.h"
#include "src/core/multispectrum.h"

QWorkspace* getworkspaceptr();

const double Fitter::eps = std::numeric_limits<double>::epsilon();

Fitter::Fitter(Model* m)
: tempspectrumptr(new Spectrum(m->getnpoints())), //make a temporary spectrum storage of the same size as model
  modelptr(m),
  deriv(modelptr->getnroffreeparameters(), modelptr->getnpoints()),
  alpha(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters()),
  modcurve(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters()),
  information_matrix(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters()),
  beta(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters()),
  residualspec(0),
  mresidualspec(0),
  curvaturematrix(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters()),
  fraction(0.001),
  minstep(1.0e-99),
  maxstep(1.0e99),
  tolerance(1.0e-10),
  nmax(100),
  usegradients(true),
  dolin(false), // force the linear trick to be on by default
  residual(false),
  domulti(false),
  status("Fitter options"),
  constructed(false),
  goodnessqueue(),
  fittertype(),
  candolin(false) //a derived fitter has to declare that he can do this speed-up trick
{
  std::cout <<"making fitter\n";

  //create vector with changeable parameters and all other matrices that depend on the number of parameters
  createmodelinfo();
  // calculate the standard deviation of the experimental datapoints, needed for some fitters to estimate the error
  // on a certain experimental value
  calculate_sigma();

  constructed=true;
}

Fitter::~Fitter() {}

double Fitter::degreesoffreedom()const{
  //calculate the degree of freedom
  //the number of (free, not excluded) points in the spectrum minus the number of free parameters
  const int n=modelptr->getnpoints();
  //check how many points are excluded
  int nfree=0;
  for (int i=0;i<n;i++){
    if (!(modelptr->isexcluded(i)))  nfree++;
  }
  const double degrees=( (double)nfree/(modelptr->getdetectorcorrfactor())-(double)modelptr->getnroffreeparameters());
  //EXPERIMENTAL...correct the number of independent pixels by the correlation factor in the detector
  //this gives an important correction for the chisq/dof and for the width of the distribution of the chisq.

  return   degrees;
}

void Fitter::createmodelinfo()
//create all links to parameters of a model, do this whenever the model has changed
{
  std::vector<Component*> allcomponents=modelptr->getcomponentvector();

  //the model should always be inited
  //modelptr->init(); //update all info related to the free parameters
  //resize or create all matrices

  // if current values need to be preserved, use conservativeResize
  curvaturematrix.resize(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());
  beta.resize(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());
  deriv.resize(modelptr->getnroffreeparameters(), modelptr->getnpoints());
  alpha.resize(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());
  modcurve.resize(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());
  information_matrix.resize(modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());

  if(constructed)
  {
    created_modelinfo(); //perform other tasks if necessery, user suplies these in derived classes
    //do this only when not called from the constructor, otherwise the derived classes are not properly constructed yet
  }
  modelptr->setchanged(false);//reset the trigger...
  #ifdef FITTER_DEBUG
  std::cout <<"recreated model info\n";
  #endif

  //resize or create all matrices
  //notify model has changed for safety, since after 1 calculation, the changed state dissapears again
  //the fitter misses it, and assumes wrong values for the freeparameters
  //modelptr->setchanged(true);
}

void Fitter::partial_derivative(size_t j,const Spectrum* currentspectrum)
{
  // create partial derivative for the model spectrum wrt a parameter Pj
  // use either the analytical expression in the component or
  // a numerical derivative
  // depends on usegradients and on the fact that the component has a getgradient

  if(usegradients && modelptr->getcomponentforfreeparam(j)->get_has_gradient(modelptr->getfreeparamindex(j)))
  {
    Spectrum* gradient=modelptr->getgradientptr(modelptr->getcomponentfreeparamindex(j),modelptr->getfreeparamindex(j));
    if(gradient == 0)
    {
      usegradients=false;
      return;
    }
    //copy it in the Xprime matrix
    for(unsigned int i=0;i<modelptr->getnpoints();i++)
    {
      if(modelptr->isexcluded(i))
        deriv(j,i) = 0.0; //no derivative in excluded regions
      else
        deriv(j,i) = gradient->getcounts(i);
    }
    return;
  }
  //numerical partial derivative to a parameter p store in row j of derivative matrix
  // deriv=(f[i](p+delta)-currentspectrum[i])/delta
  Parameter* p = modelptr->getfreeparam(j);
  const double originalvalue = p->getvalue(); //make sure originalvalue can not be changed
  double delt = fabs(originalvalue*fraction);
  if(delt < minstep)
    delt=minstep;
  if(delt > maxstep)
    delt=maxstep;
  const double delta = delt;//make sure delta can not be changed

  //add delta
  p->setvalue(originalvalue+delta);
  modelptr->calculate();
  for(int i = 0; i<modelptr->getnpoints(); ++i)
  {
    deriv(j,i) = modelptr->getcounts(i);
  }
  //reset to original, BIG advantage, the model is already calculated now, this saves time
  p->setvalue(originalvalue);
  for(int i = 0; i<modelptr->getnpoints(); ++i)
  {
    deriv(j,i) -= currentspectrum->getcounts(i);
    deriv(j,i) /= delta;
    if(modelptr->isexcluded(i))
      deriv(j,i) = 0.0; //no derivative in excluded regions
  }
}

void Fitter::calculate_derivmatrix(){
  //calculate derivative matrix
  //each row is a partial derivative of the model spectrum points to a free parameter
  modelptr->calculate();
  *tempspectrumptr=*modelptr; //copy the spectrum
  for (size_t i=0;i<modelptr->getnroffreeparameters();i++){
      partial_derivative(i,tempspectrumptr);
      }
  modelptr->calculate();
  //after calling this function you can be sure that the model is calculated in the current
  //parameters: this saves time in the optimisation
}

void Fitter::modifiedcurve(double flambda)
{
  //create the modified curvature matrix
  //the diagonal elements get larger depending on flambda
  for(int j = 0; j<alpha.rows(); ++j)
  {
    for(int k = 0; k<alpha.cols(); ++k)
    {
      modcurve(j,k) = (alpha(j,k)) / (std::sqrt(std::abs(alpha(j,j) * alpha(k,k))) + eps);//abs and eps to prevent division by zero and sqrt of negative value
    }
    modcurve(j,j) = 1.0 + flambda;
  }
}

void Fitter::newparameters()
{
  //calculate new parameters which will give a better fit
  for (size_t j=0; j<modelptr->getnroffreeparameters(); j++ ){
    double oldvalue=(modelptr->getfreeparam(j))->getvalue();
    double delta=0.0;
    for (size_t k=0; k<modelptr->getnroffreeparameters(); k++ ){
      delta += beta[k]*modcurve(j,k) / (std::sqrt(std::abs(alpha(j,j)*alpha(k,k))) + eps);//abs and eps to prevent sqrt of negative and division by zero
      }
    const double newvalue=oldvalue+delta;

    updateparam(modelptr->getfreeparam(j),newvalue); //do it but check bounds etc
  }
}

double Fitter::estimatesigma(int j) const
{
  return std::sqrt( modcurve(j,j) / (alpha(j,j)+eps) );
}

double Fitter::iteration()
{
  //do one itteration
  //lock the model so user can not add or remove components during itteration
  modelptr->setlocked(true);
  //if the model has changed (a new or removed component eg.) do an update
  if (modelptr->has_changed()){
    createmodelinfo();
  }
  calculate_derivmatrix();
  calculate_beta_and_alpha();
  #ifdef FITTER_DEBUG
  alphaptr->debugdisplay();
  #endif
  //check original goodness of fit
  //modelptr->calculate(); //don't need to because calculate_derivmatrix already did this!!!!!
  const double goodness=goodness_of_fit();
  double flambda=1.0e-6;
  double newgoodness=0.0;
  //calculate modified curvature matrix

  modelptr->storeparams(); //store current parameters

  do{
    modifiedcurve(flambda);
    //invert this matrix
    modcurve = modcurve.inverse();
    //calculate new parameters to improve the fit
    newparameters();
    modelptr->calculate();
    //if chi square increased, increase flambda and try again
    newgoodness=goodness_of_fit();
    flambda*=10.0;
    }
  while( (goodness<newgoodness) && (flambda < 10000.0) );

  if (goodness<newgoodness){
    //if the last step was bad, undo it and recalc with old params
    modelptr->retrieveparams();
    newgoodness=goodness;
  }
 
  #ifdef FITTER_DEBUG  
  flambda /= 10.0;
  std::cout <<"fitter flamda="<<flambda<<"\n";
  #endif
 //unlock the model so user can add or remove components
  modelptr->setlocked(false);

  //put the goodness in the goodnessqueue to check for convergence
  addgoodness(newgoodness);

  return newgoodness;
}

void Fitter::iterate(int n){
  //do n itterations
  for (int i=0;i<n;i++){
    iteration();
  }
}
bool Fitter::iterate_and_check(int n){
  //do n iterations but stop earlier if converged
   for (int i=0;i<n;i++){
    iteration();
    if (converged()) return true;
    }
  return false;
}

void Fitter::updateresidual(){
     //calculate the residual between model and experiment and update or create a residual (multi)spectrum

     //check if a residual spectrum already exists otherwise create one
     if (residualspec==0){
        if (modelptr->ismulti()){
           mresidualspec=new Multispectrum();
           //add same amount of spectra to it as the HLptr
           for (unsigned int i=0;i<(modelptr->getmultispectrumptr())->getsize();i++){
               Spectrum * dummy=new Spectrum((modelptr->getHLptr())->getnpoints(),(modelptr->getHLptr())->getenergy(0),(modelptr->getHLptr())->getdispersion());
               dummy->setname("Residuals");
               dummy->clear(); //fill with zero's
               mresidualspec->addspectrum(dummy);
           }
           mresidualspec->setcurrentslice((modelptr->getconstmultispectrumptr())->getcurrentslice());
           residualspec=mresidualspec->getcurrentspectrum();
           mresidualspec->setname("Residuals");
           mresidualspec->display(getworkspaceptr());
        }
        else{
             residualspec=new Spectrum((modelptr->getHLptr())->getnpoints(),(modelptr->getHLptr())->getenergy(0),(modelptr->getHLptr())->getdispersion());
             residualspec->setname("Residual");
             residualspec->display(getworkspaceptr());
        }

     }
     if (modelptr->ismulti()){
        mresidualspec->setcurrentslice((modelptr->getconstmultispectrumptr())->getcurrentslice());
        residualspec=mresidualspec->getcurrentspectrum();
     }
     //try carefully to get its name to see if it really still exists
     try{
     std::string name;
     name=residualspec->getname();
     }
     catch(...){
                residualspec=0;
                mresidualspec=0;
                return;
     }
     //make sure to point to the same slice as the current experiment

     //now fill in the residual with the current spectrum
     for (unsigned int i=0;i<(modelptr->getHLptr())->getnpoints();i++){
         const double res=(modelptr->getcounts(i))-((modelptr->getHLptr())->getcounts(i)); //model-exp
         residualspec->setcounts(i,res);
     }
     //update the graph

     if (modelptr->ismulti()){
        //update the image by reloading the mspec to a bitmap
        mresidualspec->updatereload();
     }
     else{
          residualspec->display(getworkspaceptr());
     }
}
void Fitter::calculate_sigma()const{
    //the sigma on each point is taken as the sigma of the HL spectrum and sigma=sqrt(y)
    for (unsigned int i=0;i<((modelptr->getHLptr())->getnpoints());i++){
      (modelptr->getHLptr())->seterror(i,sqrt(fabs((modelptr->getHLptr())->getcounts(i)))); //the abs is to avoid sqrt of a negative number
      }
}
bool Fitter::converged()const{
  //check if fitter is converged
  //the last two itterations, the goodness of fit should change less than tolerance
  if (goodnessqueue.size()==3){
    double g1=goodnessqueue[1];
    double g2=goodnessqueue[2];
    double g3=goodnessqueue[3];
    if ((fabs(g1-g2)<tolerance)&&(fabs(g2-g3)<tolerance)) return true;
    }
  return false;
}
void Fitter::addgoodness(double g){
  //add a new goodness_of_fit value to the goodness queue
  //a place where life is good
  //the goodness queue is a queue with 3 elements containing the three last values of
  //the goodness of fit

  if (goodnessqueue.size()<3) goodnessqueue.push_back(g);     //fill in last one
  else {
    //create my own queue since a real queue does not allow indexing (it seems)
    goodnessqueue[1]=goodnessqueue[2];
    goodnessqueue[2]=goodnessqueue[3];
    goodnessqueue[3]=g;

    }
}
int Fitter::getnumberofspectra(){
  if (getmulti()){
    //a multispectrum contains a number of spectra to fit
    return (modelptr->getmultispectrumptr())->getsize();
  }
  else{
    //a normal spectrum only has 1 spectrum to fit
    return 1;
  }
}



void Fitter::setmodelptr(Model* m){
  //change the modelpointer
  modelptr=m;
  createmodelinfo();
}

double Fitter::LRtestconfidence(){
  //return the confidence value for the LR test
   const double LR=likelihoodratio();
   const int nu=(int)degreesoffreedom();
   //compare LR result with the 1-alfa cumulative chi square distribution
   //the model is then accepted at a minimum confidence level of 1-alfa
   const double confidence=cumulative_chisquareQ(LR,nu);
   return confidence;
}

std::string  Fitter::LRtestconfidence_string(){
   //returns a string which says how good the model is
  char s[256];
  double confidence=LRtestconfidence()*100.0;
  sprintf(s,"The model is accepted at a confidence level of: %f percent",confidence);
  std::string f=s;
  return f;
}


void Fitter::created_modelinfo(){
  //nothing to do here, only derived classes put something here
}

void Fitter::initfitter(){
 //   //do nothing, this is called whenever the spectrum is changed in a multifit
    //don't redraw here, only redraw after fitting succeeded
     #ifdef FITTER_DEBUG
  std::cout <<"Fitter::initfitter called\n";
  #endif
}


 void Fitter::updateparam(Parameter* p,double newval){
     //update to a new parameter but check boundaries and step sizes


    const double oldvalue=p->getvalue();
     //check newval for Nan
    if (std::isnan(newval)){
         newval=oldvalue;
    }
     //check if delta is not too large or too small
    double delta=oldvalue-newval;

    if (fabs(delta)<minstep) delta=0.0;
    if (fabs(delta)>maxstep) delta=0.0;
    //in case the parameter is limitted
    //go to that limit but stay 2 times the derivative step from that limit
    //so a derivative can still be taken


    const double upperlimit=p->getupperbound();
    const double lowerlimit=p->getlowerbound();
    //const double range=upperlimit-lowerlimit;
    //double margin=2.0*newval*fraction;
    //make sure margin is smaller then 1/2 of the total range
    //if (margin>(0.5*range)){
    //    margin=0.5*range;
    //}

    if (p->isbound()){
      if (newval>=upperlimit){
      //we have to clip
      newval=(1.0-2.0*fraction)*upperlimit;
      }
      if (newval<=lowerlimit){
      //we have to clip
      newval=(1.0-2.0*fraction)*lowerlimit;
      }
    }
    p->setvalue(newval);
}

void Fitter::setstatus(std::string s){
    status=s;
    }
std::string Fitter::getstatus(){
    return status;
    }

void Fitter::calculate_beta_and_alpha(){
    //do nothing...the derived fitters must do smth here if they want
}
void Fitter::dolintrick(bool b){
    dolin=b;
    createmodelinfo(); //some parts may depend on the fact that we do the linear trick or not
}
bool  Fitter::getdolintrick()const{
    return dolin;
}
void Fitter::updatemonitors(){
    //call updatemonitors for each component
    modelptr->updatemonitors();
    }
