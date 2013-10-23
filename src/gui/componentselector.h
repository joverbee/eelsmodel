/***************************************************************************
                          componentselector.h  -  description
Select a component in the model
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

#ifndef COMPONENTSELECTOR_H
#define COMPONENTSELECTOR_H

#include <qwidget.h>

/**
  *@author Jo Verbeeck
  */

class Componentselector : public QWidget  {
   Q_OBJECT
public:
	Componentselector(QWidget *parent, const char *name, const char* message,int& value);
	~Componentselector();
};

#endif
