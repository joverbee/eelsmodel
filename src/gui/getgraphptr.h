/***************************************************************************
                          getgraphptr.h  -  description
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

#ifndef GETGRAPHPTR_H
#define GETGRAPHPTR_H

#define GETGRAPHDEBUG
#include <qdialog.h>
#include <qwidget.h>
class Spectrum;
class Multispectrum;
/**
  *@author Jo Verbeeck
  */

class Getgraphptr : public QDialog  {
   Q_OBJECT
  Spectrum* specptr;
  Multispectrum* mspecptr;
  bool multi;
public: 
	Getgraphptr(QWidget *parent=0, const char *name=0, const char* message=0);
	~Getgraphptr();
Spectrum* getpointer();
Multispectrum* getmultipointer();
bool ismulti();
public slots:
void slot_ok();
void slot_cancel();
};

#endif
