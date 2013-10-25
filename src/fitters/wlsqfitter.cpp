/***************************************************************************
                          wlsqfitter.cpp  -  description
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

#include "src/fitters/wlsqfitter.h"

#include <cstdio>

WLSQFitter::WLSQFitter(Model* m)
:Fitter(m)
{
settype("WLSQFitter");
m->initpoissonerror(); //fill in the error bars of this spectrum with sqrt(ppc*cts)
}

WLSQFitter::~WLSQFitter(){
}
double WLSQFitter::goodness_of_fit()const{
  //calculate the chi square
  double chisq=0.0;
  for (unsigned int i=0;i<modelptr->getnpoints();i++){
    if (!modelptr->isexcluded(i)){//only take the non-excluded points
      double weight=getweight(i);
      const double pppc=(modelptr->getHLptr())->getpppc();
      double exper=pppc*((modelptr->getHLptr())->getcounts(i));
      double fit=pppc*(modelptr->getcounts(i));
      chisq+=weight*pow((exper-fit),2.0);
      }
    }
  #ifdef FITTER_DEBUG
  std::cout <<"Weighted Chi Square: "<<chisq<<"\n";
  #endif
  return chisq;
}
std::string WLSQFitter::goodness_of_fit_string()const{
  //returns a string which says how good the fit is
  char s[256];
  double chisq=goodness_of_fit();
  sprintf(s,"Normalized Weighted Chi square: %e",chisq/(modelptr->getnpoints()));
  std::string f=s;
  return f;
}
void WLSQFitter::calculate_beta_and_alpha(){
  //calculate beta and alpha matrix
  alphaptr->clearlower();  //clear lower left corner including diagonal
  //clear beta
  for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
    beta[j]=0.0;
    }

  for (size_t i=0;i<(modelptr->getnpoints());i++){
    double expdata=(modelptr->getHLptr())->getcounts(i);
    double modeldata=modelptr->getcounts(i);
    if (!(modelptr->isexcluded(i))){ //don't count points that are excluded
      double weight=getweight(i);
      for (size_t j=0;j<modelptr->getnroffreeparameters();j++){
            beta[j] += weight*(expdata-modeldata)*((*derivptr)(j,i));
        for (size_t k=0; k<=j; k++){
         (*alphaptr)(j,k) += weight*((*derivptr)(j,i))*((*derivptr)(k,i));
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

double WLSQFitter::getweight(int index)const{
    double sigma=(modelptr->getHLptr())->geterrorbar(index);
    if (sigma==0.0) return 0.0; //if for some reason sigma is 0.0 don't use that point
    return 1.0/(pow(sigma,2.0)+eps);
}
