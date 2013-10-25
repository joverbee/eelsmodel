/***************************************************************************
                          lsqfitter.h  -  description
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

#ifndef LSQFITTER_H
#define LSQFITTER_H

//#define FITTER_DEBUG  //print some debug statistics

#include "src/fitters/fitter.h"

/* Least Squares fitter, using a Levenberg Marquardt algorithm. */

class LSQFitter : public Fitter  {
public:
LSQFitter(Model* m);
~LSQFitter();
double goodness_of_fit()const;
std::string goodness_of_fit_string()const;
void calculate_beta_and_alpha();
void CRLB(){}; //calculate with Cramer Rao Lower Bound the sigmas of the parameters
double likelihoodratio();
};

#endif
