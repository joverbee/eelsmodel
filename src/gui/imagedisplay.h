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
 * eelsmodel - gui/imagedisplay.h
 **/

#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

//#define DEBUG_IMDISPLAY

#include <QImage>
#include <QPaintEvent>
#include <QPixmap>
#include <QMouseEvent>
#include <QWidget>

#include <Eigen/Core>

#include "src/core/multispectrum.h"

class QMdiArea;

/* A class to draw images or multispectra in image format
   Is 2D equiv of Graph */

class Imagedisplay : public QWidget  {
   Q_OBJECT
QImage image;
unsigned int imwidth;
unsigned int imheight;
bool paintslice;
Multispectrum* mspecptr;
Eigen::MatrixXd* matrixptr;
std::string imdisplayname;
bool dragging;
bool is2D;
QMdiArea *parentptr;
public:
Imagedisplay(QMdiArea *parent=0, const char *name=0,Multispectrum* mspec=0);
Imagedisplay(QMdiArea *parent=0, const char *name=0, Eigen::MatrixXd* matrix = 0);
Imagedisplay(QMdiArea *parent, std::string name,size_t dim1,size_t dim2);
~Imagedisplay();
std::string getname()const;
void update();
void setname(std::string name);
void updatereloadmspec();
void updatereloadmatrix();
Multispectrum* getmultispectrumptr(){return mspecptr;};
Eigen::MatrixXd& getmatrix(){return *matrixptr;};
protected:
void paintEvent( QPaintEvent * );
bool reconvertImage();
void mousePressEvent(QMouseEvent* e);
void mouseMoveEvent(QMouseEvent* e);
void mouseReleaseEvent(QMouseEvent* e);
virtual void keyPressEvent( QKeyEvent *event );
void convertmatrixtoimage(const Eigen::MatrixXd& matrix);
void convertmspectoimage(Multispectrum * mspec);
signals:
void curr_spec_update();
};

#endif
