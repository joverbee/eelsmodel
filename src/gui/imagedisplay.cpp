/***************************************************************************
                          imagedisplay.cpp  -  description
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

#include "src/gui/imagedisplay.h"
#include <qimage.h>
#include <qlayout.h>
#include <qpixmap.h>
#include <qcolor.h>
#include <qapplication.h>
#include <qpainter.h>
#include <qpoint.h>
#include <qcursor.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include "src/core/curvematrix.h"
#include <QWorkspace>

//#ifdef DEBUG
//    #include "debug_new.h" //memory leak checker
//#endif


Imagedisplay::Imagedisplay(QWorkspace *parent, const char *name, Multispectrum* mspec)
: QWidget(parent),image()
{

    //QWidget(parent,name,Qt::WDestructiveClose)
    #ifdef DEBUG_IMDISPLAY
      std::cout << "creating image from multispectrum\n";
#endif
this->setFocusPolicy(Qt::StrongFocus ); //needed for key input
parent->addWindow(this); //add it explicitly to the workspace
paintslice=true;
mspecptr=mspec;
matrixptr=0;
dragging=false;
parentptr=parent;
setname(mspec->getname());
is2D=false;
//create the image
if (mspec->is2D()){
   is2D=true;
   imheight=mspec->getsize()/mspec->getstride();
   imwidth=mspec->getstride();
}else
{
   imwidth=mspec->getnpoints();
   imheight=mspec->getsize();
}
image=QImage(imwidth,imheight,QImage::Format_RGB32);
convertmspectoimage(mspec);
setname(name);
this->setMinimumSize(imwidth,imheight);

}
Imagedisplay::Imagedisplay(QWorkspace *parent, std::string name,size_t dim1,size_t dim2)
	: QWidget(parent),image()
	{
    matrixptr=new CurveMatrix(dim1,dim2);				     
	is2D=false;
	parent->addWindow(this); //add it explicitly to the workspace
	this->setFocusPolicy(Qt::StrongFocus ); //needed for key input
	mspecptr=0;
	paintslice=false;
	dragging=false;
	parentptr=parent;
	setname(name);
	this->setWindowTitle(name.c_str());
	//create the image
	imwidth=matrixptr->dim1();
	imheight=matrixptr->dim2();
	image=QImage(imwidth,imheight,QImage::Format_RGB32);


	//fit the data in a grayscale image
	convertmatrixtoimage(matrixptr);
	
	this->setMinimumSize(imwidth,imheight);
}

Imagedisplay::Imagedisplay(QWorkspace *parent, const char *name,CurveMatrix* matrix)
: QWidget(parent),image()
{

     //QWidget(parent,name,Qt::WDestructiveClose)
is2D=false;
parent->addWindow(this); //add it explicitly to the workspace
this->setFocusPolicy(Qt::StrongFocus ); //needed for key input
mspecptr=0;
paintslice=false;
dragging=false;
parentptr=parent;
setname(name);
this->setWindowTitle(name);
matrixptr=matrix;

//create the image
imwidth=matrix->dim1();
imheight=matrix->dim2();
image=QImage(imwidth,imheight,QImage::Format_RGB32);


//fit the data in a grayscale image
convertmatrixtoimage(matrix);
this->setMinimumSize(imwidth,imheight);

}






Imagedisplay::~Imagedisplay(){
    //if the imagedisplay closes, also kill the multispectrum that is attached to it
    if (mspecptr!=0) delete(mspecptr);
}

void Imagedisplay::convertmatrixtoimage(CurveMatrix* matrix){
     //copy and scale contents from the mspectrum in image
     const double min=matrix->getmin();
     const double max=matrix->getmax();
     #ifdef DEBUG_IMDISPLAY
     std::cout<<"min: "<<min<<"  max: "<<max<<"\n";
     #endif

     imwidth=matrix->dim1();
     imheight=matrix->dim2();
     image=QImage(imwidth,imheight,QImage::Format_RGB32);

     for (unsigned int x=0;x<imwidth;x++){
         for (unsigned int y=0;y<imheight;y++){
             //adjust the display value to fit into the range 0-255 in BW mode
             double color=(*matrix)(x,y);
             color=((color-min)/(max-min))*255.0;
             int icolor=int(color);
             QColor rgbcolor=QColor(icolor,icolor,icolor);
             image.setPixel(x,y,rgbcolor.rgb());
         }
     }
     //adapt the size of this widget to fit around the image
     //this->setFixedSize (image.width(),image.height()); //was setFixedSize
     //convert image to pixmap
     //this->reconvertImage();
     //set the caption
    

     //set the cursor of this widget as an up/down arrow to drag the current spectrum line up and down
     this->setCursor(Qt::SplitHCursor);

     //draw the image
     this->show();
}

void Imagedisplay::convertmspectoimage(Multispectrum* mspec){
//copy and scale contents from the mspectrum in image
const int min=int(mspec->getmin());
const int max=int(mspec->getmax());
#ifdef DEBUG_IMDISPLAY
std::cout<<"min: "<<min<<"  max: "<<max<<"\n";
#endif

for (unsigned int x=0;x<imwidth;x++){
    for (unsigned int y=0;y<imheight;y++){
      //adjust the display value to fit into the range 0-255 in BW mode
      size_t id=0;
      size_t eid=0;
      if (is2D){
          id=x+y*imwidth;
          eid=0; //take first pixel of the 2D SI as display of grayscale
      }
      else{
          id=y;
          eid=x;
          }
      double color=(mspec->getspectrum(id))->getcounts(eid);
      color=((color-min)/(max-min))*255.0;
      int icolor=int(color);
      QColor rgbcolor=QColor(icolor,icolor,icolor);
      image.setPixel(x,y,rgbcolor.rgb());
      }
    }
    this->setWindowTitle(getname().c_str());

    //set the cursor of this widget as an up/down arrow to drag the current spectrum line up and down
    if (is2D){
        this->setCursor(Qt::CrossCursor);
    }
    else
    {
        this->setCursor(Qt::SplitVCursor);
    }
    //draw the image
    this->show();
}

void Imagedisplay::paintEvent( QPaintEvent * e){
QPainter painter(this);
//int side=QMIN(this->width(), this->height());
painter.setViewport(0,0,this->width(),this->height()); //defines the range of the real coordinates
painter.setWindow(0,0,imwidth,imheight); //defines the range of logical coordinates (used for drawing, in pixels of the image
//this->setWindowTitle(getname().c_str());

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
void Imagedisplay::update(){

  this->repaint();
}

void Imagedisplay::updatereloadmspec(){
//reconvert the image from the matrix and repaint
  convertmspectoimage(mspecptr);
  this->repaint();
}

void Imagedisplay::updatereloadmatrix(){
//reconvert the image from the matrix and repaint
  convertmatrixtoimage(matrixptr);
  this->repaint();
}

void Imagedisplay::setname(std::string name){
  imdisplayname=name;
}

std::string Imagedisplay::getname()const{
  std::string result="";
  if (mspecptr!=0){
     result=(mspecptr->getcurrentspectrum())->getname();
  }


  return result;
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


