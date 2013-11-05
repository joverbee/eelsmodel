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
 * eelsmodel - fitters/mlfitter.cpp
 **/

#include "src/fitters/mlfitter.h"

//#define FITTER_DEBUG

#include <cstdio>
#include <cmath>

#include "src/core/model.h"
#include "src/core/monitor.h"

MLFitter::MLFitter(Model* m)
:Fitter(m)
{
settype("MLFitter");
}
MLFitter::~MLFitter(){
}
double MLFitter::goodness_of_fit()const{
  return -likelyhoodfunction();
}
double MLFitter::likelyhoodfunction()const{
  //calculate the real likelyhood
  //the probability that this experiment was created by this model with these parameters
  double likelyhood=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!(modelptr->isexcluded(i))){//only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      //made use of stirling formula
      //log(n!)~nlog(n)-n for large n
      //if ((fit>0.0)&&(exper>0.0)) likelyhood+=(exper*log(fit)-fit-(exper*log(exper)-exper));
      if ((fit>1.0)&&(exper>1.0)) {

        //likelyhood+=(exper*log(fit)-fit-(exper*log(exper)-exper));

        //like it was up till 8-3-2007
        likelyhood+=2.0*(exper*log(fit/exper)-fit+exper); //numerically better to do ln(fit/exper) because fit~=exper

         //better approx in Stirling ln(n!)=nlnn-n+0.5*ln(2 pi n)
         //gives wrong result in LR since formula there assumes old Stirling formula
        //likelyhood+=2.0*(exper*log(fit/exper)-fit+exper-0.5*log(6.2832*exper)); //numerically better to do ln(fit/exper) because fit~=exper

        //factor of 2 is to have a good comparison with chisquare values
        //and this is same as the likelihood ratio this number should be roughly equal to degrees of freedom
        //for more accurate statement see LR comments
       }
      if (fit<0.0){
                 likelyhood=-(std::numeric_limits<double>::max)(); //a model that goes negative is not possible for Poisson, return -infty
      }
      }
    }
  //correct likelihood for correlated noise
  //be carefull no to do this in the fischer information matrix
  //otherwise the precisions are wrong
  //EXPERIMENTAL!!!!!!!!!!!!!!!!!!!!!
  #ifdef FITTER_DEBUG
  std::cout <<"real likelyhood before correction corrfactor: "<<likelyhood<<" the corrfactor="<<(modelptr->getdetectorcorrfactor())<<"\n";
  #endif
  //likelyhood=likelyhood*modelptr->getdetectorcorrfactor();
  likelyhood=likelyhood; //adjust the dof formula instead, this scales also the chisq distribution


//EXPERIMENTAL!!!!!!!!!!!!!!!!!
  #ifdef FITTER_DEBUG
  std::cout <<"real likelyhood: "<<likelyhood<<"\n";
  #endif
  return likelyhood;
}
std::string MLFitter::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  //divide by number of points...TO DO
  double likelyhood=-likelyhoodfunction();
  const double n=this->degreesoffreedom();
  likelyhood=likelyhood/n;
  sprintf(s,"Likelihood merrit function (similar to chisq/dof): %e",likelyhood);
  std::string f=s;
  return f;
}
void MLFitter::calculate_beta_and_alpha(){
  //calculate beta and alpha matrix
  alphaptr->clearlower();  //clear lower left corner including diagonal
  //clear beta
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    beta[j]=0.0;
    }

  for (unsigned int i=0;i<(modelptr->getnpoints());i++){
    if (!(modelptr->isexcluded(i))){ //don't count points that are excluded
      double expdata=fabs((modelptr->getHLptr())->getcounts(i));
      double modeldata=fabs(modelptr->getcounts(i)); //make sure modeldata is realy positive
        for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
           //adapted version of beta for ML
           beta[j] += 0.5*((expdata-modeldata)/(modeldata+eps))*((*derivptr)(j,i));
           //original for lsq
           //beta[j] +=      (expdata-modeldata)                 *((*derivptr)[j][i]);
           for (size_t k=0; k<=j; k++){
              //adapted version of alpha matrix for ML
              (*alphaptr)(j,k) += 0.5*(expdata/(pow(modeldata,2.0)+eps))*((*derivptr)(j,i))*((*derivptr)(k,i));
              //original for lsq
              //(*alphaptr)[j][k] +=                                        ((*derivptr)[j][i]*(*derivptr)[k][i]);
              }
           }
      }

    }

  //copy the one triangle to the other side because of symmetry
  for (size_t j=1; j<modelptr->getnroffreeparameters(); j++){
  //was j=0 but first row needs not to be copied because is already full
      for (size_t k=0; k<j; k++){
        //was k<=j but you don't need to copy the diagonal terms
        ((*alphaptr)(k,j)) = ((*alphaptr)(j,k));
        }
      }
}

void MLFitter::CRLB(){
//get the covariance matrix by inverting the Fischer Matrix
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
    modelptr->getfreeparam(j)->setsigma(sqrt(variance));
    }
}
double MLFitter::getcovariance(int i,int j){
  //before using this, set up the covariance matrix with preparecovariance()
  double result=0.0;
  try{
    //be carefull...
    result=(*information_matrix)(i,j);
  }
  catch(...){
    return 0.0;
  }
  return result;
}

void MLFitter::preparecovariance(){
  //prepare covariance matrix by inverting the Fischer Information Matrix

 //calculate the Fischer information matrix
 for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    for (size_t k=0;k<modelptr->getnroffreeparameters();k++){
       (*information_matrix)(j,k)=0.0;
        for (unsigned int i=0;i<modelptr->getnpoints();i++){
          //only take non-excluded points
          if (!modelptr->isexcluded(i)){
            double modeldata=fabs(modelptr->getcounts(i)); //make sure modeldata is realy positive
            (*information_matrix)(j,k)+=(1.0/(modeldata+eps))*((*derivptr)(j,i))*((*derivptr)(k,i));
          }
        }
    }
  }
  #ifdef FITTER_DEBUG
  std::cout <<"The Fischer information matrix:\n";
  information_matrix->debugdisplay();
  #endif
  //invert the Fischer matrix to get the lower bound for the variance
  information_matrix->inv_inplace();

  #ifdef FITTER_DEBUG
  std::cout <<"The inverted Fischer information matrix:\n";
  information_matrix->debugdisplay();
  #endif
}

double MLFitter::likelihoodratio(){
  //calculate the likelyhood ratio (LR)
  //this number should be compared to the chi square distribution with
  //n-k degrees of freedom (n=number of points to be fitted, k number of parameters)
  double LR=-likelyhoodfunction(); //formula's are the same

  /*
  for (unsigned int i=0;i<(modelptr->getnpoints());i++){
    if (!(modelptr->isexcluded(i))){
      //only take the non-excluded points
      const double exper=(HLptr->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      if ((fit>0.0)&&(exper>0.0)) {
          LR+=2.0*((fit-exper)+exper*log(exper/fit));//see paper Arian Den Decker (TU Delft)

          //LR+=pow(exper-fit,2.0)/fit; //paper Arian for (small) observation values
          //made use of stirling formula
        }
      }
    }
    */
  #ifdef FITTER_DEBUG
  std::cout <<"likelyhood ratio is: "<<LR<<"\n";
  std::cout <<"the degrees of freedom: "<<this->degreesoffreedom()<<"\n";
  #endif
  return LR;
}
void MLFitter::updatemonitors(){
  //fill in the CRLB values for each free parameter in the sigma porperty of the param
  this->CRLB();
  //CRLB already uses the covariance matrix so we don't need to recalculate it
  //update the covariances for each parameter that has a monitor
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    if (modelptr->getfreeparam(j)->ismonitored()){
      Monitor* mymonitor=(modelptr->getfreeparam(j))->getmonitor();
      //find the index of the 2n paramters of the monitor
      Parameter* mypar2=mymonitor->getpar2();
      bool found=false;
      size_t i=0;
      for (i=0;i<modelptr->getnroffreeparameters();i++){
          if (mypar2==modelptr->getfreeparam(i)){
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
