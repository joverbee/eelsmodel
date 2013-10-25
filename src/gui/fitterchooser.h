/***************************************************************************
                          fitterchooser.h  -  description
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

#ifndef FITTERCHOOSER_H
#define FITTERCHOOSER_H

#include <QWidget>

#include "src/fitters/fitter.h"

/* A Qwidget to choose the type of fitter to use. */

class FitterChooser : public QWidget  {
   Q_OBJECT
public: 
	FitterChooser(QWidget *parent, const char *name,int& retvalue);
	~FitterChooser();
};

#endif
