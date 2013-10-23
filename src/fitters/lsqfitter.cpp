/***************************************************************************
                          lsqfitter.cpp  -  description
                             -------------------
    begin                : Sat Nov 23 2002
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

#include "src/fitters/lsqfitter.h"
#include <cstdio>

#ifndef linux
//#include <stdio>
#endif



LSQFitter::LSQFitter(Model* m)
:Fitter(m)
{
 settype("LSQFitter");
}
LSQFitter::~LSQFitter(){
}
double LSQFitter::goodness_of_fit()const{
  //calculate the chi square
  double chisq=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!(modelptr->isexcluded(i))){//only take the non-excluded points
      const double exper=modelptr->getHLptr()->getcounts(i);
      const double fit=modelptr->getcounts(i);
      chisq+=pow((exper-fit),2.0);
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"Chi Square: "<<chisq<<"\n";
  #endif
  return chisq;
}
std::string LSQFitter::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  double chisq=goodness_of_fit();
  sprintf(s,"Chisq/dof: %e",chisq/(this->degreesoffreedom()));
  std::string f=s;
  return f;
}
void LSQFitter::calculate_beta_and_alpha(){
  //calculate beta and alpha matrix
  alphaptr->clearlower();  //clear lower left corner including diagonal
  //clear beta
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    beta[j]=0.0;
    }

  for (unsigned int i=0;i<(modelptr->getnpoints());i++){
    double expdata=(modelptr->getHLptr())->getcounts(i);
    double modeldata=modelptr->getcounts(i);
    if (!(modelptr->isexcluded(i))){ //don't count points that are excluded
      for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
            beta[j] += (expdata-modeldata)*((*derivptr)(j,i));
        for (size_t k=0; k<=j; k++){
         (*alphaptr)(j,k) += ((*derivptr)(j,i))*((*derivptr)(k,i));
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

double LSQFitter::likelihoodratio(){
  //calculate the likelyhood ratio (LR)
  //this number should be compared to the chi square distribution with
  //n-k degrees of freedom (n=number of points to be fitted, k number of parameters)
  //the model is a good approximation of the experiment if the likelyhood-ratio is to be compared
  //with the chi square cumulative distribution for a given confidence level
  double LR=0.0;
  for (size_t i=0;i<(modelptr->getnpoints());i++){
    if (!(modelptr->isexcluded(i))){
      //only take the non-excluded points
      const double exper=((modelptr->getHLptr())->getcounts(i));
      const double fit=(modelptr->getcounts(i));
      if ((fit>0.0)&&(exper>0.0)) {
        LR+=pow(exper-fit,2.0);//see paper Arian
        }
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"likelihood ratio is: "<<LR<<"\n";
  #endif
  return LR;
}
