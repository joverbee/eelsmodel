/***************************************************************************
                          chisquare.h  -  description
some simple routines for calculating the cumulative chi square distribution
partly taken from netlib
                             -------------------
    begin                : Mon Jun 23 2003
    copyright            : (C) 2003 by Jo Verbeeck
    email                : joverbee@ruca.ua.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef CHISQUARE_H
#define CHISQUARE_H

double pochisq (double x,int df);
double critchi (double p,int df);
double poz(double z);

double cumulative_chisquare(double x,int nu);
double cumulative_chisquareQ(double x,int nu);

#endif
