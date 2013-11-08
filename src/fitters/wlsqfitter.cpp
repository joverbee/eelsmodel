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
 * eelsmodel - fitters/wlsqfitter.cpp
 **/

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
  //clear lower left corner of alpha including diagonal
  for(int i = 0; i<alpha.rows(); ++i)
  {
    for(int j = 0; j<=i; ++j)
    {
      alpha(i,j)=0;
    }
  }
  //clear beta
  beta = 0;

  for(size_t i = 0; i<modelptr->getnpoints(); ++i)
  {
    double expdata = modelptr->getHLptr()->getcounts(i);
    double modeldata = modelptr->getcounts(i);
    if(!(modelptr->isexcluded(i))) //don't count points that are excluded
    {
      double weight = getweight(i);
      for(size_t j = 0; j<modelptr->getnroffreeparameters(); ++j)
      {
        beta[j] += weight * (expdata-modeldata) * deriv(j,i);
        for(size_t k = 0; k<=j; ++k)
        {
          alpha(j,k) += weight * deriv(j,i) * deriv(k,i);
        }
      }
    }
  }
  //TODO use alpha.triangularView<Lower>() instead
  //copy the one triangle to the other side because of symmetry
  for(size_t j = 1; j<modelptr->getnroffreeparameters(); j++)
  {
    //was j=0 but first row needs not to be copied because is already full
    for(size_t k = 0; k<j; ++k)
    {
      //was k<=j but you don't need to copy the diagonal terms
      alpha(k,j) = alpha(j,k);
    }
  }
}

double WLSQFitter::getweight(int index)const{
    double sigma=(modelptr->getHLptr())->geterrorbar(index);
    if (sigma==0.0) return 0.0; //if for some reason sigma is 0.0 don't use that point
    return 1.0/(pow(sigma,2.0)+eps);
}
