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
 * eelsmodel - gui/equalizer.h
 *
 * A widget to graphically control the value of a large set of parameters.
 **/

#ifndef EQUALIZER_H
#define EQUALIZER_H

#include <QSlider>
#include <QWidget>

class QWorkspace;

class Component;
class Parameter;

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
