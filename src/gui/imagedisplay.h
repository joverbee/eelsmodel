/***************************************************************************
                          imagedisplay.h  -  description
                             -------------------
    begin                : Thu Mar 6 2003
    copyright            : (C) 2003 by Jo Verbeeck
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

#ifndef IMAGEDISPLAY_H
#define IMAGEDISPLAY_H

//#define DEBUG_IMDISPLAY


#include <qwidget.h>
#include <qimage.h>
#include <qpixmap.h>
//Added by qt3to4:
#include <QMouseEvent>
#include <QPaintEvent>

#include "src/core/multispectrum.h"
#include "src/core/curvematrix.h"
class QWorkspace;
/**A class to draw images or multispectra in image format
Is 2D equiv of Graph
  *@author Jo Verbeeck
  */

class Imagedisplay : public QWidget  {
   Q_OBJECT
QImage image;
unsigned int imwidth;
unsigned int imheight;
bool paintslice;
Multispectrum* mspecptr;
CurveMatrix* matrixptr;
std::string imdisplayname;
bool dragging;
bool is2D;
QWorkspace *parentptr;
public:
Imagedisplay(QWorkspace *parent=0, const char *name=0,Multispectrum* mspec=0);
Imagedisplay(QWorkspace *parent=0, const char *name=0,CurveMatrix *matrix=0);
Imagedisplay(QWorkspace *parent, std::string name,size_t dim1,size_t dim2);
~Imagedisplay();
std::string getname()const;
void update();
void setname(std::string name);
void updatereloadmspec();
void updatereloadmatrix();
Multispectrum* getmultispectrumptr(){return mspecptr;};
CurveMatrix* getmatrix(){return matrixptr;};
protected:
void paintEvent( QPaintEvent * );
bool reconvertImage();
void mousePressEvent(QMouseEvent* e);
void mouseMoveEvent(QMouseEvent* e);
void mouseReleaseEvent(QMouseEvent* e);
virtual void keyPressEvent( QKeyEvent *event );
void convertmatrixtoimage(CurveMatrix * matrix);
void convertmspectoimage(Multispectrum * mspec);
signals:
void curr_spec_update();
};

#endif
