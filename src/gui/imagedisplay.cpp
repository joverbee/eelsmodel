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
 * eelsmodel - gui/imagedisplay.cpp
 **/

#include "src/gui/imagedisplay.h"

#include <string>
  using std::string;

#include <QApplication>
#include <QColor>
#include <QCursor>
#include <QImage>
#include <QLayout>
#include <QPainter>
#include <QPixmap>
#include <QPoint>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QWorkspace>

#include <Eigen/Dense>

//#ifdef DEBUG
//    #include "debug_new.h" //memory leak checker
//#endif

Imagedisplay::Imagedisplay(QWorkspace *parent, const QString& name, Multispectrum* mspec)
: QWidget(parent),
  image(convertmspectoimage(*mspec)),
  imwidth(image.width()),
  imheight(image.height()),
  paintslice(true),
  mspecptr(mspec),
  matrixptr(0),
  imdisplayname(name),
  is2D(mspec->is2D())
{
#ifdef DEBUG_IMDISPLAY
  std::cout << "creating image from multispectrum\n";
#endif
  setFocusPolicy(Qt::StrongFocus ); //needed for key input
  parent->addWindow(this); //add it explicitly to the workspace

  setname(mspec->getname().c_str());

  setMinimumSize(imwidth,imheight);

  setWindowTitle(getname());

  //set the cursor of this widget as an up/down arrow to drag the current spectrum line up and down
  if(is2D)
    setCursor(Qt::CrossCursor);
  else
    setCursor(Qt::SplitVCursor);
  //draw the image
  show();
}

Imagedisplay::Imagedisplay(QWorkspace *parent, const QString& name,size_t dim1,size_t dim2)
: QWidget(parent),
  image(convertmatrixtoimage(Eigen::MatrixXd(dim1,dim2))),
  imwidth(image.width()),
  imheight(image.height()),
  paintslice(false),
  mspecptr(0),
  matrixptr(0),
  imdisplayname(name),
  dragging(false),
  is2D(false)
{
	parent->addWindow(this); //add it explicitly to the workspace

  setFocusPolicy(Qt::StrongFocus ); //needed for key input
  setWindowTitle(name);
  setMinimumSize(image.width(),image.height());
  //set the cursor of this widget as an up/down arrow to drag the current spectrum line up and down
  setCursor(Qt::SplitHCursor);

  //adapt the size of this widget to fit around the image
  //this->setFixedSize (image.width(),image.height()); //was setFixedSize
  //convert image to pixmap
  //this->reconvertImage();
  //set the caption

  //draw the image
  show();
}

Imagedisplay::Imagedisplay(QWorkspace *parent, const QString& name, Eigen::MatrixXd* matrix)
: QWidget(parent),
  image(convertmatrixtoimage(*matrix)),
  paintslice(false),
  mspecptr(0),
  matrixptr(matrix),
  imdisplayname(name),
  dragging(false),
  is2D(false)
{
  parent->addWindow(this); //add it explicitly to the workspace

  setFocusPolicy(Qt::StrongFocus ); //needed for key input
  setWindowTitle(name);
  setMinimumSize(image.width(),image.height());
  //set the cursor of this widget as an up/down arrow to drag the current spectrum line up and down
  setCursor(Qt::SplitHCursor);

  //adapt the size of this widget to fit around the image
  //this->setFixedSize (image.width(),image.height()); //was setFixedSize
  //convert image to pixmap
  //this->reconvertImage();
  //set the caption

  //draw the image
  show();
}

Imagedisplay::~Imagedisplay(){
    //if the imagedisplay closes, also kill the multispectrum that is attached to it
    if (mspecptr!=0) delete(mspecptr);
}

QImage convertmatrixtoimage(const Eigen::MatrixXd& matrix)
{
  //copy and scale contents from the matrix in image
  const double min=matrix.minCoeff();
  const double max=matrix.maxCoeff();
#ifdef DEBUG_IMDISPLAY
  std::cout<<"min: "<<min<<"  max: "<<max<<"\n";
#endif

  int imwidth=matrix.rows();
  int imheight=matrix.cols();
  QImage image = QImage(imwidth, imheight, QImage::Format_RGB32);

  for(int x=0; x<imwidth; ++x)
  {
    for(int y=0; y<imheight; ++y)
    {
      //adjust the display value to fit into the range 0-255 in BW mode
      double color=matrix(x,y);
      color=((color-min)/(max-min))*255.0;
      int icolor=int(color);
      QColor rgbcolor=QColor(icolor,icolor,icolor);
      image.setPixel(x,y,rgbcolor.rgb());
    }
  }
  return image;
}

QImage convertmspectoimage(const Multispectrum& mspec)
{
  int imheight;
  int imwidth;
  if(mspec.is2D())
  {
    imheight=mspec.getsize()/mspec.getstride();
    imwidth=mspec.getstride();
  }
  else
  {
    imwidth=mspec.getnpoints();
    imheight=mspec.getsize();
  }
  QImage image(imwidth, imheight, QImage::Format_RGB32);
  //copy and scale contents from the mspectrum in image
  const int min=int(mspec.getmin());
  const int max=int(mspec.getmax());
#ifdef DEBUG_IMDISPLAY
  std::cout<<"min: "<<min<<"  max: "<<max<<"\n";
#endif

  for(int x=0;x<imwidth;x++)
  {
    for(int y=0;y<imheight;y++)
    {
      //adjust the display value to fit into the range 0-255 in BW mode
      size_t id=0;
      size_t eid=0;
      if(mspec.is2D())
      {
        id=x+y*imwidth;
        eid=0; //take first pixel of the 2D SI as display of grayscale
      }
      else
      {
        id=y;
        eid=x;
      }
      double color=(mspec.getspectrum(id))->getcounts(eid);
      color=((color-min)/(max-min))*255.0;
      int icolor=int(color);
      QColor rgbcolor=QColor(icolor,icolor,icolor);
      image.setPixel(x,y,rgbcolor.rgb());
    }
  }
  return image;
}

void Imagedisplay::paintEvent(QPaintEvent* e)
{
  QPainter painter(this);
  painter.setViewport(0,0,width(),height()); //defines the range of the real coordinates
  painter.setWindow(0,0,image.width(),image.height()); //defines the range of logical coordinates (used for drawing, in pixels of the image
  setWindowTitle(getname());

        painter.setClipRect(e->rect());
        //painter.drawPixmap(0,0,pm);

        //image.scaled(this->size());
         painter.drawImage(0,0,image);

        //paint a red line over the slice that is current in the mspectrum
        if (paintslice){
          painter.setPen(QColor("red"));

          if (is2D){
            //paint a red square around the pixel that is currently selected
            const int w=1;
            const int h=1;
            int y1=0;
            int x1=0;
            if (mspecptr!=0){
                 y1=mspecptr->getcurrentslice()/imwidth;
                 x1=mspecptr->getcurrentslice()-y1*imwidth;
            }
            painter.drawRect(x1,y1,w,h);
          }
          else{
            //paint a red line over the slice that is current in the mspectrum
            const int x1=0;
            const int x2=imwidth-1;
            int y1=0;
            if (mspecptr!=0) y1=mspecptr->getcurrentslice();
            const int y2=y1;
            painter.drawLine(x1,y1,x2,y2);
          }
          }
}
void Imagedisplay::update()
{
  repaint();
}

void Imagedisplay::updatereloadmspec()
{
  //reconvert the image from the matrix and repaint
  image = convertmspectoimage(*mspecptr);
  repaint();
}

void Imagedisplay::updatereloadmatrix()
{
  //reconvert the image from the matrix and repaint
  image = convertmatrixtoimage(*matrixptr);
  repaint();
}

void Imagedisplay::setname(const QString& name)
{
  imdisplayname=name;
}

QString Imagedisplay::getname() const
{
  if(mspecptr!=0)
    return mspecptr->getcurrentspectrum()->getname();
  else
    return "";
}

void Imagedisplay::mousePressEvent(QMouseEvent* e){
  //override the qwidget mousepressEvent
  //if the left mouse button was pressed
  dragging=true;
}
void Imagedisplay::mouseMoveEvent(QMouseEvent* e){
  //override the qwidget mouseMoveEvent
  QPoint thispos=e->pos();
  //convert to viewport coordinates
  unsigned int y=(unsigned int)(double(thispos.y()*imheight)/double(this->height()));
  unsigned int x=(unsigned int)(double(thispos.x()*imwidth)/double(this->width()));
  size_t id=y;
  if (is2D){
    id=x+y*imwidth;
  }

  //check if we are in grabbing mode
  if (dragging){
    if (mspecptr!=0) mspecptr->setcurrentslice(id);//this does an automatic update
    emit(curr_spec_update()); //tell eelsmodel that the current slice has changed
    }
}

void Imagedisplay::mouseReleaseEvent(QMouseEvent* e){
  dragging=false;
  if (mspecptr!=0) mspecptr->update();
}

void Imagedisplay::keyPressEvent(QKeyEvent *event )
{
#ifdef DEBUG_IMDISPLAY
      std::cout << "some key has been pressed\n";
#endif

   if( event->key()==Qt::Key_Down) {
#ifdef DEBUG_IMDISPLAY
      std::cout << "arrow down key has been pressed\n";
#endif
      //increase current spectrum by 1
      if (mspecptr!=0){
        unsigned int cslice=mspecptr->getcurrentslice();
        if (cslice!=mspecptr->getsize()){
            cslice=cslice+1;
        }
        mspecptr->setcurrentslice(cslice);
        emit(curr_spec_update()); //tell eelsmodel that the current slice has changed
        mspecptr->update();
        }
   }
   if( event->key()==Qt::Key_Up) {
#ifdef DEBUG_IMDISPLAY
      std::cout << "arrow up key has been pressed\n";
#endif
     //decrease current spectrum
      if (mspecptr!=0){
        unsigned int cslice=mspecptr->getcurrentslice();
        if (cslice!=0){
            cslice=cslice-1;
        }
        mspecptr->setcurrentslice(cslice);
        emit(curr_spec_update()); //tell eelsmodel that the current slice has changed, this will retrieve the parameters and do a recalc of the model
        mspecptr->update();
        }

   }

}


