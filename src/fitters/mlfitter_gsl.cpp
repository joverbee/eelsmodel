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
 * eelsmodel - fitters/mlfitter_gsl.h
 **/

 //A wrapper to use the GSL nonlinear fitting routines to get a more robust (but slower) result
//as a first test only least square, ML would require small changes in the GSL code

#include "src/fitters/mlfitter_gsl.h"

//#define COPYING //do this only if the fast pointer way of working seems to create problems

namespace GSL
{
  #include  <gsl/gsl_multifit_nlin.h>
  #include <gsl/gsl_blas.h>
}

#include "src/core/model.h"
#include "src/core/monitor.h"

#include "src/gui/saysomething.h"

//C-style functions required for call-back, the this pointer is passed as a parameter
//to simulate member-type functionality
int model_f(const gsl_vector * x, void * params, gsl_vector * f);
int model_f_norecalc(const gsl_vector * x, void * params, gsl_vector * f);
int model_df (const gsl_vector * x, void *params,gsl_matrix * J);
int model_fdf (const gsl_vector * x, void *params,gsl_vector * f, gsl_matrix * J);
//static gsl_matrix_view Jv; //matrix view of the real derivmatrix, so we don't need to copy it to pass as gsl_matrix

MLFitterGSL::MLFitterGSL(Model* m)
:Fitter(m)
{
  constructed=false;
  #ifdef FITTER_DEBUG
  std::cout <<"creating MLFitter_GSL\n";
  #endif
  covar=0;
  initialparameters=0;
 //Jv=gsl_matrix_view_array (0, 0, 0);

  solver=0;
  dummypar=this;
  settype("MLFitterGSL");
  //init fitter and memory for x and covar
  initfitter(); //already called via createmodelinfo() which calls created_modelinfo() which call initfitter()
  constructed=true;
}
MLFitterGSL::~MLFitterGSL(){
  //free up allocated memory
  if (solver!=0) gsl_multifit_fdfsolver_free (solver);
  if (initialparameters!=0) gsl_vector_free (initialparameters);
  if (covar!=0) gsl_matrix_free (covar);
}
double MLFitterGSL::goodness_of_fit()const{
  const double chisq=likelihoodfunction();
  #ifdef FITTER_DEBUG
  std::cout <<"likelihood goodness of fit: "<<chisq<<"\n";
  #endif
  return chisq;
}
double MLFitterGSL::likelihoodfunction()const{
  //return the likelihood according to the solver
  return pow(GSL::gsl_blas_dnrm2(solver->f),2.0);
  //the excluded points have f=0.0 anyway so they don't play a role here.   
}

double MLFitterGSL::likelihoodfunctionML()const{
  //calculate the real likelihood
  //the probability that this experiment was created by this model with these parameters
  double likelihood=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!(modelptr->isexcluded(i))){//only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      if ((fit>1.0)&&(exper>1.0)) {
        likelihood+=2.0*(exper*log(fit/exper)-fit+exper); //numerically better to do ln(fit/exper) because fit~=exper
       }
       if (fit<0.0){
                       likelihood=-(std::numeric_limits<double>::max)(); //a model that goes negative is not possible for Poisson, return -infty
       }
      }
    }
  return likelihood;
}

std::string MLFitterGSL::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  sprintf(s,"Likelihood merrit function: %e",-likelihoodfunctionML()/degreesoffreedom());
  std::string f=s;
  return f;
}
void MLFitterGSL::calculate_beta_and_alpha(){
   //nothing to do since the GSL routines do this inside
}

void MLFitterGSL::CRLB(){
//get the covariance matrix from the GSL routines
//the diagonal elements now contain the limit of the variance on a parameter
//if the fit would be repeated an infinite amount of times
//on data captured in exactly the same circumstances (only different in noise)
// the distribution around the "real" parameter is gaussian

//reset all sigmas in the model
  modelptr->resetsigmas();
//fill out the sigmas for the free paramaters
  //setup the covariance matrix
  preparecovariance();
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    double variance=fabs(getcovariance(j,j)); //avoid sqrt of neg with abs
    //store this in the sigma value of the parameters
    (modelptr->getfreeparam(j))->setsigma(sqrt(variance));
    }
}
double MLFitterGSL::getcovariance(const int i,const int j){
  //before using this, set up the covariance matrix with preparecovariance()
  double result=0.0;
  try{
    //be carefull...
    result=gsl_matrix_get(covar,i,j);
  }
  catch(...){
    return 0.0;
  }
  return result;
}

void MLFitterGSL::preparecovariance(){
  //prepare covariance matrix by inverting the Fischer Information Matrix
  const double epsrel=1e-32;
  //calculate covar matrix from this
  gsl_multifit_covar (solver->J, epsrel,covar);
}

double MLFitterGSL::likelihoodratio(){
  //calculate the likelihood ratio (LR)
  //this number should be compared to the chi square distribution with
  //n-k degrees of freedom (n=number of points to be fitted, k number of parameters)
  double LR=0.0;
  for (unsigned int i=0;i<(modelptr->getnpoints());i++){
    if (!(modelptr->isexcluded(i))){
      //only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      if ((fit>0.0)&&(exper>0.0)) {
        LR+=2.0*(-exper+exper*log(exper)+fit-exper*log(fit));//see paper Arian
        //LR+=pow(exper-fit,2.0)/fit; //paper Arian for small observation values
        }
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"likelihood ratio is: "<<LR<<"\n";
  #endif
  return LR;
}
void MLFitterGSL::updatemonitors(){
  //fill in the CRLB values for each free parameter in the sigma porperty of the param
  this->CRLB();
  //CRLB already uses the covariance matrix so we don't need to recalculate it
  //update the covariances for each parameter that has a monitor
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    if ((modelptr->getfreeparam(j))->ismonitored()){
      Monitor* mymonitor=(modelptr->getfreeparam(j))->getmonitor();
      //find the index of the 2n paramters of the monitor
      Parameter* mypar2=mymonitor->getpar2();
      bool found=false;
      size_t i=0;
      for (i=0;i<modelptr->getnroffreeparameters();i++){
          if (mypar2==(modelptr->getfreeparam(i))){
             found =true;
             break;
             }
        }
      if (found){
        mymonitor->setcovariance(this->getcovariance(j,i));
      }
      else{
        mymonitor->setcovariance(0.0); //parameter 2 is not free so covariance is 0.0
      }
    }
  }

}

void MLFitterGSL::initfitter(){
    //do first whatever the fitter wants to do here
    Fitter::initfitter();

  #ifdef FITTER_DEBUG
  std::cout <<"mlfitter_gsl::initfitter called\n";
  #endif

  //createmodelinfo(); //update number of free parameters etc if a model has changed
  const int n=modelptr->getnpoints();
  try{
    //delete old solver and create new one
    if (solver!=0) gsl_multifit_fdfsolver_free (solver);
    const gsl_multifit_fdfsolver_type * T = gsl_multifit_fdfsolver_lmsder; //Scaled Levenberg Marquardt
    solver= gsl_multifit_fdfsolver_alloc (T,n,modelptr->getnroffreeparameters());

    //reallocate memory for parameter vector x and covariance matrix covar
    if (initialparameters!=0) gsl_vector_free (initialparameters);
    if (covar!=0) gsl_matrix_free (covar);

    initialparameters=gsl_vector_calloc (modelptr->getnroffreeparameters()); //allocate space for vector x, containing param values
    covar=gsl_matrix_calloc (modelptr->getnroffreeparameters(), modelptr->getnroffreeparameters());

    //init the link with the function to be fitted
    initfdf();

    //load the current parameters in the x vector
    initparameters();

    //reinit the solver
    gsl_multifit_fdfsolver_set(solver,&f,initialparameters);
  }
  catch(...){
    //problems with memory? -> kill this
    Saysomething mysay(0,"Error","Unable to allocate memory, exit MLFitterGSL",true);
    delete(this);
  }
}

double MLFitterGSL::iteration(){
  //do one itteration
  //lock the model so user can not add or remove components during itteration
  modelptr->setlocked(true);
  //if the model has changed (a new or removed component eg.) create a new solver
  if (modelptr->has_changed()){
      createmodelinfo();     //this automaticaly calls initfitter() via created_modelinfo()
  }
  //do an itteration step  
  try{
    const int status=gsl_multifit_fdfsolver_iterate(solver);
    #ifdef FITTER_DEBUG
    std::cout <<"solver status: "<<gsl_strerror(status)<<"\n";
    std::cout <<"chi square/dof: "<<gsl_blas_dnrm2(solver->f)<<"\n";
    #endif
    this->setstatus(gsl_strerror(status)); //update status info of fitter
    convertxtoparam(solver->x);
    
    
    
  }
  catch(...){
    Saysomething mysay(0,"Error","Problem with call to gsl_multifit_fdfsolver_iterate, exit MLFitterGSL",true);
    delete(this);
  }
 //unlock the model so user can add or remove components
  modelptr->setlocked(false);
  //put the goodness in the goodnessqueue to check for convergence
  const double newgoodness= goodness_of_fit();
  addgoodness(newgoodness);
  return newgoodness;
}

void MLFitterGSL::initfdf(){ //function to be minimized and derivatives
    //initialise a fdf function which is linked to our model
    //stupid coying of data->is this the advantage of using a library????

  f.f = &(model_f);
  f.df = &(model_df);
  f.fdf = &(model_fdf);
  f.n = modelptr->getnpoints();
  f.p = modelptr->getnroffreeparameters();
  f.params = this; //store pointer to this in the params



}

int model_f(const gsl_vector * x, void * params, gsl_vector * f){
  //this function should store the vector result f(x,params) in f for argument x and parameters params,
  //returning an appropriate error code if the function cannot be computed.

  //CONFUSING
  //in our nomenclature
  //x contains the parameter values
  //params contains extra info, we store the this pointer to able to use static function and still have a reference to our class
  //store in f the values of the function to be minimised at given parameters
  MLFitterGSL* pointertothis=(MLFitterGSL *)params;
  pointertothis->convertxtoparam(x);
  #ifdef FITTER_DEBUG
  std::cout <<"calling the user function from: "<<pointertothis->gettype()<<"\n";
  #endif
  //calculate the model
  (pointertothis->modelptr)->calculate();
  //copy the result of the likelihoodfunction in f

  for (size_t i=0;i<(pointertothis->modelptr)->getnpoints();i++){  
      const double deviation=pointertothis->functionvalue(i);
      gsl_vector_set(f,i,deviation);  
  }

  //alternative without copying, should be much faster
  //read info on vector_view

  //this can not work because we only want to pass the nonexcluded points
  //we don't have these in a separate vector

  //so keep doing it the stupid way

  return GSL_SUCCESS;
}

int model_f_norecalc(const gsl_vector * x, void * params, gsl_vector * f){
  //this function should store the vector result f(x,params) in f for argument x and parameters params,
  //returning an appropriate error code if the function cannot be computed.

  //CONFUSING
  //in our nomenclature
  //x contains the parameter values
  //params contains extra info, we store the this pointer to able to use static function and still have a reference to our class
  //store in f the values of the function to be minimised at given parameters
  MLFitterGSL* pointertothis=(MLFitterGSL *)params;
  pointertothis->convertxtoparam(x);
  #ifdef FITTER_DEBUG
  std::cout <<"calling the user function from: "<<pointertothis->gettype()<<"\n";
  #endif

  //no calc of the model since df already did this

  //copy the result of the likelihoodfunction in f
  for (size_t i=0;i<(pointertothis->modelptr)->getnpoints();i++){
      const double deviation=pointertothis->functionvalue(i);//returns 0.0 for exluded points
      gsl_vector_set(f,i,deviation);
  }
  return GSL_SUCCESS;
}



int model_df (const gsl_vector * x, void *params,gsl_matrix * J)
{
  //store the derivative matrix in J
  MLFitterGSL* pointertothis=(MLFitterGSL *)params;
  pointertothis->convertxtoparam(x);
  pointertothis->calculate_derivmatrix();

  //copy this in J
  for(size_t i = 0; i<pointertothis->modelptr->getnpoints(); ++i)
  {
    for(size_t j = 0; j<pointertothis->modelptr->getnroffreeparameters(); ++j)
    {
      const double d=pointertothis->deriv(j,i);
      //#ifdef FITTER_DEBUG
      //std::cout <<"J["<<i<<"]["<<j<<"]= "<<d<<"\n";
      //#endif
      gsl_matrix_set(J,i,j,d);
    }
  }



  //make a matrix_view, so we don't need to copy this, but only copy the pointer to the derivmatrix

  //this doesn't work because Murphy decided that the matrix GSL wants is the transpose of the matrix I have
  //changing this order
  //Jv=gsl_matrix_view_array(&(pointertothis->derivptr->array())[0][0],pointertothis->derivptr->dim1(),pointertothis->derivptr->dim2());
  //J=&Jv.matrix;





  return GSL_SUCCESS;
}

int model_fdf (const gsl_vector * x, void *params,gsl_vector * f, gsl_matrix * J)
{
  //do df first, this already calculates the model, so we can save some time
  model_df (x, params, J);
  model_f_norecalc(x,params,f);   
  return GSL_SUCCESS;
}


void MLFitterGSL::convertxtoparam(const gsl_vector * x){
  //copy the parameter values to the model
  for (size_t i=0; i < modelptr->getnroffreeparameters();i++){      
      const double newval=gsl_vector_get(x,i);
      updateparam(modelptr->getfreeparam(i),newval);
  }
  //there is no way I see to pass the parameters as pointers since they
  //come from a vector of pointers to parameters which are not consecutive in memory
  //I see no way to make a gsl vector point to the right memory locations since
  //it only wants base adress and stride and offset etc...
}
void MLFitterGSL::initparameters(){
  //copy values of current parameters to initialparameters
  const size_t free=modelptr->getnroffreeparameters(); //test for debugger
  for (size_t i=0; i < free;i++){
    gsl_vector_set(initialparameters,i,(modelptr->getfreeparam(i))->getvalue());
  }
  //there is no way is see to pass the parameters as pointers since they
  //come from a vector of pointers to parameters which are not consecutive in memory
  //I see no way to make a gsl vector point to the right memory locations since
  //it only want base adress and stride and offset etc...
}
void MLFitterGSL::created_modelinfo() {
  //do extra tasks when createmodelinfo() was called
  //in this case we have to tell the gsl solver that things have changed
   initfitter();
}
double MLFitterGSL::functionvalue(size_t i) {
//return the function value at point i
  if (modelptr->isexcluded(i)) return 0.0; //don't return deviation in exluded points
  const double exper=(modelptr->getHLptr())->getcounts(i);
  const double fit=modelptr->getcounts(i);
  //simple least squares
  const double deviation=fit-exper;  //Carefull!!!use this definition instead of negated
  //OTHERWISE TROUBLE WITH DERIVATIVES THAT DON'T MATCH, THIS IS DIFFICULT TO FIND OUT SINCE THE
  //FITTER DOES NOT PERFORM THE STEP WHEN THE FIT GETS WORSE
  //read between the lines (some precious Sunday time was wasted...)
  return deviation;
}



