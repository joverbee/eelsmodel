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
 * eelsmodel - fitters/wlsqfitter.h
 **/

#ifndef WLSQFITTER_H
#define WLSQFITTER_H

#include "src/fitters/fitter.h"
#include "src/core/model.h"

/* Weighted Least Squares Fitter. */

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
