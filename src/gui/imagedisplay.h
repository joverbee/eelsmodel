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

#include "src/core/multispectrum.h"

class QWorkspace;

/* A class to draw images or multispectra in image format
   Is 2D equiv of Graph */

QImage convertmatrixtoimage(const Eigen::MatrixXd& matrix);
QImage convertmspectoimage(const Multispectrum& mspec);

class Imagedisplay : public QWidget
{
  Q_OBJECT

public:
  Imagedisplay(QWorkspace *parent, const std::string& name, Multispectrum* mspec);
  Imagedisplay(QWorkspace *parent, const std::string& name, Eigen::MatrixXd* matrix);
  Imagedisplay(QWorkspace *parent, std::string name,size_t dim1,size_t dim2);
  ~Imagedisplay();
  std::string getname()const;
  void update();
  void setname(std::string name);
  void updatereloadmspec();
  void updatereloadmatrix();

private:
  QImage image;
  unsigned int imwidth;
  unsigned int imheight;
  bool paintslice;
  Multispectrum* mspecptr;
  Eigen::MatrixXd* matrixptr;
  std::string imdisplayname;
  bool dragging;
  bool is2D;
  QWorkspace *parentptr;

  Multispectrum* getmultispectrumptr(){return mspecptr;};
  Eigen::MatrixXd* getmatrix(){return matrixptr;};

protected:
  void paintEvent(QPaintEvent* event);
  bool reconvertImage();
  void mousePressEvent(QMouseEvent* event);
  void mouseMoveEvent(QMouseEvent* event);
  void mouseReleaseEvent(QMouseEvent* event);
  virtual void keyPressEvent(QKeyEvent *event);

signals:
  void curr_spec_update();
};

#endif
