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
 * Copyright: 2003-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - gui/realinput.h
 **/

#ifndef REALINPUT_H
#define REALINPUT_H

#include <QWidget>

/* Ask for a real number input from user. */

class RealInput : public QWidget  {
   Q_OBJECT
public: 
RealInput(QWidget *parent, const char *name,const char* message,double& value,const double min=0.0,const double max=1.0);
~RealInput();
};

#endif
