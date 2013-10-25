/***************************************************************************
                          monitorchooser.h  -  description
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

#ifndef MONITORCHOOSER_H
#define MONITORCHOOSER_H

#include <QWidget>

/* A Qwidget to choose the type of monitor to use. */

class MonitorChooser : public QWidget  {
   Q_OBJECT
public:
MonitorChooser(QWidget *parent, const char *name,int& retvalue);
	~MonitorChooser();
};

#endif
