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
 * eelsmodel - gui/fitterchooser.h
 **/

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
