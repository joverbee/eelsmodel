/***************************************************************************
                          saysomething.h  -  description
                             -------------------
    begin                : Sat Oct 26 2002
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

#ifndef SAYSOMETHING_H
#define SAYSOMETHING_H

#include <qwidget.h>

/**
  *@author Jo Verbeeck
  */

class Saysomething : public QWidget  {
   Q_OBJECT
public: 
	Saysomething(QWidget *parent=0, const char *name=0, const char* message=0,bool error=false);
	~Saysomething();
};

#endif
