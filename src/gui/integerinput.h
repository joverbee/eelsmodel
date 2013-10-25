/***************************************************************************
                          integerinput.h  -  description
Ask for an integer value
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

#ifndef INTEGERINPUT_H
#define INTEGERINPUT_H

#include <QWidget>

class Integerinput : public QWidget  {
   Q_OBJECT
public: 
	Integerinput(QWidget *parent, const char *name, const char* message,int& value,const int min,const int max);
	~Integerinput();
};

#endif
