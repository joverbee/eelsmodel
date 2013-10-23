/***************************************************************************
                          wlsqfitter.h  -  description
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

#ifndef WLSQFITTER_H
#define WLSQFITTER_H

#include <src/fitters/fitter.h>
#include "src/core/model.h"
/**Weighted Least Squares Fitter
  *@author Jo Verbeeck
  */

class WLSQFitter : public Fitter  {

public: 
	WLSQFitter(Model* m);
	~WLSQFitter();
double goodness_of_fit()const;
std::string goodness_of_fit_string()const;
void calculate_beta_and_alpha();
double getweight(int index)const;
void CRLB(){}; //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double likelihoodratio(){return 0.0;} ; //not yet implemented
};

#endif
