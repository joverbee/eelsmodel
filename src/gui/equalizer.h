/***************************************************************************
                          equalizer.h  -  description
A widget to graphically control the value of a large set of parameters
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

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <qwidget.h>

/**
  *@author Jo Verbeeck
  */
class Component;
class Parameter;
#include <qslider.h>

class QWorkspace;

class Equalizer : public QWidget  {
   Q_OBJECT
   Component* comp;
   std::vector<QSlider*> sliderlist;
   int maxval;
   size_t nparam;
public slots:
void slot_sliderrelease() ;
void slot_sliderchange();

public:
	Equalizer(QWorkspace *parent, const char *name,Component* cp);
	~Equalizer();
	void paintEvent( QPaintEvent * );
    void slidersetup();
    int getscaledvalue(Parameter* p);
    void setscaledvalue(Parameter* p,int value);
    size_t activesliderindex();

};

#endif
