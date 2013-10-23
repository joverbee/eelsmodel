/***************************************************************************
                          realinput.h  -  description
                             -------------------
    begin                : Wed May 14 2003
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

#ifndef REALINPUT_H
#define REALINPUT_H

#include <qwidget.h>

/**Ask for a real number input from user 
  *@author Jo Verbeeck
  */

class RealInput : public QWidget  {
   Q_OBJECT
public: 
RealInput(QWidget *parent, const char *name,const char* message,double& value,const double min=0.0,const double max=1.0);
~RealInput();
};

#endif
