/***************************************************************************
                          gettopspectrum.h  -  description
                             -------------------
    begin                : Sat Dec 7 2002
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

#ifndef GETTOPSPECTRUM_H
#define GETTOPSPECTRUM_H

#include <QWidget>

class Spectrum;

/* gets the spectrum of the top active window if it is a graph, otherwise 0 is returned. */

class Gettopspectrum   {
public: 
	Gettopspectrum();
	~Gettopspectrum();
Spectrum* getpointer();
};

#endif
