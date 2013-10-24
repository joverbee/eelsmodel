/***************************************************************************
                          GraphQwt.cpp  -  class to draw 2D GraphQwts
                             -------------------
    begin                : Sat Oct 12 2002
    copyright            : (C) 2002 by Jo Verbeeck
    email                : jo@localhost.localdomain
 ***************************************************************************/
// attempt to use Qwt in the graph class
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

//#define GraphQwt_DEBUG



#include "src/gui/graphqwt.h"
#include <qapplication.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <stdlib.h>                             // defines rand() function
#include <qmatrix.h>
#include <cmath>
#include <qfont.h>
#include <qwidget.h>
#include "src/core/spectrum.h"
#include "src/core/multispectrum.h"
#include <iostream>
#include "src/gui/saysomething.h"
#include "src/gui/menu.h"
#include <QWorkspace>
#include <QRubberBand>
#include <vector>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>
#include <qwt_series_data.h>

//#define GraphQwt_DEBUG


extern QWorkspace* getworkspaceptr();
extern MenuEelsmodel* getmenuptr();
//
// GraphQwt - a widget that draws a GraphQwt.
//

//
// Constructs a GraphQwt widget.
//

GraphQwt::GraphQwt( QWorkspace *parent, const char *name,Spectrum *spec)
 :QWidget(parent),data(1, std::vector<QPoint>(spec->getnpoints())),myworld(),stylelist()
 // data()
{
    myPlot = new QwtPlot(QwtText(name),parent);

    this->setWindowTitle(name);
    #ifdef GraphQwt_DEBUG
    std::cout << "Constructor of GraphQwt\n";
    std::cout << "parent adres is "<<parent<<"\n";
    std::cout << "spectrum adres is: "<<spec<<"\n";
    std::cout << "The size of data is :"<<data.size()<<"\n";
    std::cout << "The size of data[0] is:"<<data[0].size()<<"\n";
    #endif
  npoints=spec->getnpoints();
  parent->addWindow(this); //add it explicitly to the workspace (important in QT4)
 // this->setAccessibleName (QString::fromStdString(name) );
  multispectrum=false; //it's a normal spectrum
  spectrumptr=spec;
  multispectrumptr=0;

  nplots=1;
  //resize data field for containing the GraphQwt
  data.clear(); //start with empty data
  data.resize(nplots);
  data[nplots-1].resize(npoints);
    stylelist.resize(nplots);
    stylelist[nplots-1]=1; //default style

  copydata(0,spectrumptr); //copy the spectrum data into the GraphQwt data field (integer conversion!)
  setdefaults(); //set default border styles and colors etc
  scale(); //scale the data to fit in the GraphQwt, DO this after setdefaults()
  getmainwindowptr(); //get a pointer to the main window
  Init();
    #ifdef GraphQwt_DEBUG
    std::cout << "End of constructor of GraphQwt\n";
    #endif
}



GraphQwt::GraphQwt( QWorkspace *parent, const char *name,Multispectrum *mspec)
   : QWidget(parent),data(mspec->getsize(), std::vector<QPoint>(mspec->getnpoints())),myworld(),stylelist()
   // data()
{
    myPlot = new QwtPlot(QwtText(name),parent);
     this->setWindowTitle(name);
      #ifdef GraphQwt_DEBUG
    std::cout << "Constructor of GraphQwt for multispectrum\n";
    std::cout << "parent adres is "<<parent<<"\n";
    std::cout << "multispectrum adres is: "<<mspec<<"\n";
    std::cout << "The size of data is :"<<data.size()<<"\n";
    std::cout << "The size of data[0] is:"<<data[0].size()<<"\n";
    #endif

  parent->addWindow(this); //add it explicitly to the workspace
  multispectrum=true; //it's a multispectrum
  spectrumptr=mspec->getcurrentspectrum(); //a pointer to the current active spectrum
  multispectrumptr=mspec; //a pointer to the whole multispectrum
  scalefactor=1.0;
  nplots=1;
  //resize data field for containing the GraphQwt
  data.clear(); //start with empty data
  data.resize(nplots);
  npoints=spectrumptr->getnpoints();
  data[nplots-1].resize(npoints);
  stylelist.resize(nplots);
  stylelist[nplots-1]=1; //default style

  copydata(0,spectrumptr); //copy the spectrum data into the GraphQwt data field (integer conversion!)

  setdefaults(); //set default border styles and colors etc
  scale(); //scale the data to fit in the GraphQwt, DO this after setdefaults()
  getmainwindowptr(); //get a pointer to the main window
  Init();
}

GraphQwt::~GraphQwt(){
//can't delete spectrum since spectrum deletes its GraphQwt when closed
//this gives infinite recursion
       if (rubberrect!=0){
          delete(rubberrect);
       }
       delete(myPlot);
}

void GraphQwt::Init(){
     #ifdef GraphQwt_DEBUG
    std::cout << "Start of init\n";
    #endif

  //init a pointer to a rubberband for selection purposes
  rubberrect=new QRubberBand(QRubberBand::Rectangle,this);
  rubberrect->hide();
  //rubberrect->setGeometry(0,0,0,0);
}

void GraphQwt::setdefaults(){
      #ifdef GraphQwt_DEBUG
    std::cout << "Start of setdefaults\n";
    #endif
// min size for this widget
 this->setMinimumSize(300,150);
// defaults for borders etc
  border=0.05;
  xtickcount=10;
  ytickcount=10;
  ticksize=0.005;
  fontsize=0.03;
  precision=3;
  xlabel="energy";
  ylabel="counts";

   #ifdef GraphQwt_DEBUG
    std::cout << "Before reseting grabbing and area selection\n";
    #endif
//reset all grabbing and arrea selection
  grabbing=false;
  grabrect=QRect(0,0,0,0);
  setselection(false);
  setstartindex(0);
  setendindex(npoints);
  rubberrect=0;
   #ifdef GraphQwt_DEBUG
    std::cout << "Before reseting zoom\n";
       std::cout << "npoints="<<npoints<<"\n";

    #endif
//reset zoom
  setstartzoomindex(0);
  setendzoomindex(npoints-1);
  #ifdef GraphQwt_DEBUG
  std::cout << "endzoomindex="<<endzoomindex<<"\n";
  std::cout << "startzoomindex="<<startzoomindex<<"\n";
    std::cout << "before defining colors\n";
    #endif
//define default colors
  setnormalcolor(QColor("blue"));
  setexcludecolor(QColor("cyan")); //a darker version of normalcolor
  setaxiscolor(QColor("black"));
  setbgcolor(QColor("white"));
  #ifdef GraphQwt_DEBUG
    std::cout << "Before mycolors\n";
    #endif
  mycolors.clear();
  mycolors.push_back(QColor("red"));
  mycolors.push_back(normalcolor);
  mycolors.push_back(QColor("green"));
  dotsize=10; //size of a dot in the plot with style=2 in pixels
  #ifdef GraphQwt_DEBUG
    std::cout << "End of setdefaults\n";
    #endif
}

void GraphQwt::getmainwindowptr(){
      #ifdef GraphQwt_DEBUG
    std::cout << "Start of getmainwindowptr\n";
    #endif
  //get a pointer to the main window to make possible to ask
  //if we are in zoom or exclude regions mode
  try{
    mainwindow=getmenuptr();//make sure to enable RTTI in BCC32 otherwise an error about polymorphism is generated
    }
  catch(...){
    Saysomething mysay(0,"error","Something went wrong when trying to get the mainwidget in GraphQwt");
    throw;
    }
}
void GraphQwt::copydata(int layer,Spectrum* spec){
 //copy the data from the spectrum into the data field
 //THE DATA IS CLIPPED TO INT!
 //a rescaling is done when the numbers are smaller than minpoints
 //it has only a cosmetical effect, and only works for single layer plots
 const double minpoints=4096.0; //minimum number of steps between 0 and max to allow scaling of a spec with a small dynamic range
   //Determine scalefactor from layer 0 (spectrum stored in spectrumptr)
   scalefactor=1.0;
   const double range=(spectrumptr->getmax()-spectrumptr->getmin()); //always use the scale of layer 0
   if (range<minpoints){
     const double minrange=1e-6;
     if (range>minrange){
        scalefactor=fabs(minpoints/range);
     }
     else{
          scalefactor=1.0;
     }
#ifdef GraphQwt_DEBUG
 std::cout <<"spec->getmax():"<< spec->getmax() <<"\n";
 std::cout <<"spec->getmin():"<< spec->getmin() <<"\n";
 std::cout <<"range:"<< range <<"\n";
 std::cout <<"scalefactor:"<< scalefactor <<"\n";
#endif

 }
 else{//if a multilayer plot
           //keep same scalefactor as for first layer
   //scalefactor=1.0;
}

 for (size_t i=0;i<spec->getnpoints();i++){
     const double ydata=spec->getcounts(i);
     const double xdata=spec->getenergy(i);
     //careful check for overflow before doing int cast, otherwise an exception is thrown which
     //apparently can not be caught (why???)
    (data[layer][i]).setX(spectrumptr->getenergyindex(xdata));

     if ((int(ydata*scalefactor)<INT_MIN)||(int(ydata*scalefactor)>INT_MAX)){
       //put zero in case of a nan or floating point error
       (data[layer][i]).setY(0);
     }
     else{
       (data[layer][i]).setY(int(ydata*scalefactor));
     }
     //and copy in the qwt specific store
     qwtdata[i].setX(spectrumptr->getenergyindex(xdata));
     qwtdata[i].setY(spectrumptr->getenergyindex(ydata));


  }
#ifdef GraphQwt_DEBUG
 std::cout <<"end of copydata\n";
#endif
}



void GraphQwt::reinit(){
//reinitialise this GraphQwt to a new spectrum in the multispectrum
//used when a multispectrum changes its current spectrum
 if (multispectrumptr==0) return;
 spectrumptr=multispectrumptr->getcurrentspectrum();
 copydata(0,spectrumptr); //copy the new data into layer 0, other layers are not affected
 scale();
 repaint();
}

void GraphQwt::addGraphQwt(Spectrum *spec)
{
  nplots++;
  data.resize(nplots);    //increase size of data vector
  qwtdata.resize(spec->getnpoints());
  data[nplots-1].resize(spec->getnpoints());
  stylelist.resize(nplots);
  stylelist[nplots-1]=1; //the default style
  #ifdef GraphQwtDEBUG
   std::cout <<"addGraphQwt function\n";
  std::cout <<"the new size of data is "<<data.size()<<"\n";
   std::cout <<"the new size of data[0] is "<<data[0].size()<<"\n";
  #endif
  //add the data in the new layer
  copydata(nplots-1,spec);
  scale();
}
void GraphQwt::removelastGraphQwt(){
    nplots--;
  data.resize(nplots);    //decrease size of data vector
  stylelist.resize(nplots);

  #ifdef GraphQwtDEBUG
   std::cout <<"removelastGraphQwt function\n";
  std::cout <<"the new size of data is "<<data.size()<<"\n";
   std::cout <<"the new size of data[0] is "<<data[0].size()<<"\n";
  #endif
}


void GraphQwt::setstyle(int layer,size_t style){
    //set the plotstyle of a GraphQwt
    stylelist[layer]=style;
}

void GraphQwt::updateGraphQwt(int layer,Spectrum* spec){
    //check if valid layer
    if  (!this->validlayer(layer)) return;
    //add the data
    copydata(layer,spec);
    scale();
    repaint();
}
bool GraphQwt::validlayer(int layer)const{
return ((layer>=0)&&(layer<nplots));
}

void GraphQwt::scale(){
#ifdef GraphQwt_DEBUG
    std::cout << "Starting scale\n";
    std::cout << "nplots "<<nplots<<"\n";
    std::cout << "startzoomindex "<<getstartzoomindex()<<"\n";
    std::cout << "endzoomindex "<<getendzoomindex()<<"\n";
    std::cout << "spectrumptr "<<spectrumptr<<"\n";
    std::cout << "spectrumptr->getname "<<spectrumptr->getname()<<"\n";
#endif
//update xmax and xmin
  xmin=spectrumptr->getenergy(getstartzoomindex());
  #ifdef GraphQwt_DEBUG
  std::cout<<"before copy xmin: "<<xmin<<"\n";
  #endif
  xmax=spectrumptr->getenergy(getendzoomindex());
  #ifdef GraphQwt_DEBUG
  std::cout<<"before copy xmax: "<<xmax<<"\n";
  #endif
//update the ymax and ymin

  ymax=0.0;
  ymin=0.0;
  double ydata=0;
  for (int layer=0;layer<(nplots);layer++){
      for (size_t i=getstartzoomindex(layer);i<=getendzoomindex(layer);i++){
       	const int y=(data[layer][i]).y();
        ydata=double(y);
        if (ydata>ymax) {ymax=ydata;}
        if (ydata<ymin) {ymin=ydata;}
        }
      }
  #ifdef GraphQwt_DEBUG
  std::cout<<"ymin: "<<ymin<<" ymax: "<<ymax<<" nplots:"<<nplots<<"\n";
  #endif
}
void GraphQwt::setxlabel(const char* xl){xlabel=xl;}

void GraphQwt::setylabel(const char* yl){ylabel=yl;}

void GraphQwt::updateCaption(){}

void GraphQwt::paintEvent( QPaintEvent * )
{
    QPainter paint(this);
    paint.setRenderHint(QPainter::NonCosmeticDefaultPen,true); //cosmetic width of 0 doesn't work on os x
    this->setWindowTitle(spectrumptr->getname());
    //determine size of current window
    int w = width();
    int h = height();
    if(w <= 0 || h <= 0)
        return;

     //set some colors and fonts to use

                     
    paint.setBrush(bgcolor);// set random brush color
    QFont myfont=QFont(); //get default font
    
    //myfont.System;
    myfont.setPixelSize (int(double(fontsize*double(h))));
    paint.setFont (myfont);
    
    paint.setPen(normalcolor);
    QString label;


    //determine size of printed text
    int fontheight,fontwidth;
    double dummy=-1.2356e34;
    label.setNum(dummy,'g',precision); //create text with required precision and fontsize
    QRect rt;
    //draw text in center of screen
    rt=paint.boundingRect (w/2,h/2, w, h,Qt::AlignLeft|Qt::AlignBottom,label);
    fontheight=rt.height();
    fontwidth=rt.width();

    //calculate the border
    int xborder,yborder;
    xborder=fontwidth+int(double(border)*double(w));
    yborder=int(double(border)*double(h));

    int top,left,bottom,right,gw,gh;
    int xmargin,ymargin;
    xmargin=fontwidth+2*fontheight;
    ymargin=3*fontheight;
    left=xmargin;
    top=yborder;
    right=w-xborder;
    bottom=h-ymargin;
    gw=w-xborder-xmargin;//width and height of GraphQwt window
    gh=h-yborder-ymargin;

    //draw the GraphQwt region
    QPoint p1(left,top);        // p1 = top left
    QPoint p2(right,bottom);    // p2 = bottom right
    r=QRect( p1, p2 );
    

    paint.setPen(axiscolor);
    paint.drawRect( r );


    //plot the data
    double sx,sy,dx,dy;

    //double sizex,sizey;
    double sizex=xmax-xmin;
    double sizey=ymax-ymin;

        int npoints_zoom=1+getendzoomindex()-getstartzoomindex(); //size in pixels after zooming, pixel size is also size of bins in layer 0
        sx=double(gw)/double(npoints_zoom);
        sy=-double(gh)/sizey;  //put origin of y at bottom instead of top
        dx=double(xmargin)-double(getstartzoomindex())*sx;
        dy=double(yborder+gh);

        //scale the data to fit on the screensize
        //use worldmatrices
        myworld.reset();
        myworld.translate(dx,dy);
        myworld.scale(sx,sy);
        myworld.translate(0,-ymin);  //put lowest y value at the bottom of GraphQwt
        paint.setWorldTransform( myworld );


    for (int layer=nplots-1;layer>=0;layer--){

       for (size_t index=getstartzoomindex(layer);index<=getendzoomindex(layer);index++){
           if ((spectrumptr->isexcluded(index))&&(layer==1)) {
                paint.setPen(excludecolor);
           }
        else {
             if ((int)mycolors.size()>layer){
                         paint.setPen(mycolors[layer]);
             }
             }

                if (stylelist[layer]==1){
                    //draw a line
                    if ((index+1)<data[layer].size()){
                        paint.drawLine(data[layer][index],data[layer][index+1]);
                    }
                }

                else{
                    //draw a circle on the datapoints
                     if ((index)<data[layer].size()){
                         const double totx=double(npoints_zoom);
                         const int dotx=int(double(dotsize));
                         const int doty=int(double(dotsize)*sizey/totx);
                         const int width=dotx;
                         const int height=doty;
                         
                        paint.drawEllipse ( data[layer][index].x()-width/2,data[layer][index].y()-height/2,width ,height);
                        //draw a cross in the middle
                        paint.drawLine(data[layer][index].x(),data[layer][index].y()-height/2,data[layer][index].x(),data[layer][index].y()+height/2);
                        paint.drawLine(data[layer][index].x()-width/2,data[layer][index].y(),data[layer][index].x()+width/2,data[layer][index].y());
                     }
                    }


        }
      }


    paint.setPen(normalcolor);
     
    myworld.reset();
    paint.setWorldTransform( myworld );

    //create axis labels and tick marks
    double xtick,ytick,xticklabel,yticklabel;
    xticklabel=(xmax-xmin)/double(xtickcount-1.0);
    yticklabel=(ymax-ymin)/(double(ytickcount-1.0)*scalefactor); // take into account the scalefactor to have the proper labels
    xtick=double(gw)/double(xtickcount-1.0);
    ytick=double(gh)/double(ytickcount-1.0);
    int tx,ty,xticklength,yticklength;
    xticklength=int(ticksize*double(h));
    yticklength=int(ticksize*double(w));

    // x tick marks + labels
    ty=h-ymargin;
     paint.setPen(axiscolor);
    for (int i=0;i<xtickcount;i++)
    {
    tx=xmargin+int(double(i)*xtick);
    p1.setX(tx);
    p2.setX(tx);
    p1.setY(ty);
    p2.setY(ty+yticklength);
    paint.drawLine(p1,p2);
    label.setNum(xmin+double(i)*xticklabel,'g',precision);
    paint.drawText (tx,ty+yticklength, fontwidth, fontheight,Qt::AlignCenter|Qt::AlignBottom,label);
    //was -1,0

    }
    // y tick marks + labels
    tx=xmargin;
  
    paint.setPen(axiscolor);
    for (int i=0;i<ytickcount;i++)
    {
    ty=(h-ymargin)-int(double(i)*ytick);
    p1.setX(tx);
    p2.setX(tx-xticklength);
    p1.setY(ty);
    p2.setY(ty);
    paint.drawLine(p1,p2);

    label.setNum(ymin/scalefactor+double(i)*yticklabel,'g',precision);
    paint.drawText (tx-fontwidth-2,ty, fontwidth, fontheight,Qt::AlignLeft|Qt::AlignBottom,label);
     //paint.drawText (tx-fontwidth-2,ty, fontwidth, fontheight,Qt::AlignLeft|Qt::AlignBottom,label,-1,0 );
    }

    // draw xlabel and ylabel in middle of axis
    myfont.bold();
    paint.setFont (myfont);
    tx=xmargin+gw/2;
    ty=bottom+2*fontheight;
    paint.drawText (tx,ty, fontwidth, fontheight,Qt::AlignCenter|Qt::AlignBottom,xlabel );

    tx=0;
    ty=yborder+gh/2;
    //draw this text vertical
    myworld.reset();
    myworld.translate(tx,ty);
    myworld.rotate(-90);
    myworld.translate(-tx,-ty);
    paint.setWorldTransform( myworld );
    paint.drawText (tx,ty, fontwidth, fontheight,Qt::AlignLeft|Qt::AlignBottom,ylabel);
    myworld.reset();
    paint.setWorldTransform( myworld );

    //myworld.reset();
    //on top of the data draw a selection window if one exists
    if (getselected()){
     QPoint topleft(convert_index_to_coords(getstartindex()),r.top());
     QPoint bottomright(convert_index_to_coords(getendindex()),r.bottom());
     grabrect=QRect(topleft,bottomright);
     rubberrect->setGeometry(grabrect);
     rubberrect->show();
     //paint.drawRect(grabrect);//draw a new one
      }



    //try the qwt variant


    // add curves
    QwtPlotCurve *curve1 = new QwtPlotCurve("Experiment");

    // connect or copy the data to the curves
    //




    curve1->setSamples(qwtdata);
    //curve1->attach(myPlot);


    // finally, refresh the plot
    myPlot->replot();
}

Spectrum* GraphQwt::getspectrumptr(){
  return spectrumptr;
}
Multispectrum* GraphQwt::getmultispectrumptr(){
  if (multispectrum) return multispectrumptr;
  else return 0;
}
void GraphQwt::slot_GraphQwt_clicked(){
    QPainter paint( this );
    QPoint p1(0,0);        // p1 = top left
    QPoint p2(20,20);    // p2 = bottom right
    QRect r( p1, p2 );
    (void) r; // get rid of unused variable warning
    //paint.drawRect( r );
}

void GraphQwt::mousePressEvent(QMouseEvent* e){
  //override the qwidget mousepressEvent
  //if the left mouse button was pressed
  if (e->button()==Qt::LeftButton){
    startpos=e->pos();
    //check if we are in Zoom mode
    if ((mainwindow->zoomMode())||(mainwindow->selectMode())){
      //see if mouse is really in the GraphQwt rectangle
      if (r.contains ( startpos,FALSE )) {
        grabbing=true;
	rubberrect->show();
        setselection(false);//we are making a new selection, the old one is obsolete
        if (mainwindow->selectMode()){
           QCursor crosshair(Qt::CrossCursor);
          this->setCursor (crosshair);
        }
        if (mainwindow->zoomMode()){
          QCursor crosshair(Qt::CrossCursor);
          this->setCursor (crosshair);
        }
        }
      }
    }
  if (e->button()==Qt::RightButton){
    if (mainwindow->zoomMode()){
      //right clicking in zoom mode returns to normal zoom state
      setstartzoomindex(0);
      setendzoomindex(npoints-1);
      this->scale();
      this->repaint();
    }
  }


}

void GraphQwt::mouseMoveEvent(QMouseEvent* e){
  //override the qwidget mouseMoveEvent
  QPoint thispos=e->pos();
  //check if we are in grabbing mode
  if (grabbing){
     setselection(false);//we are making a new selection, the old one is obsolete
    //start dragging a rectangle
    //see if mouse is in the GraphQwt rectangle
   if (r.contains ( thispos,FALSE )) {
    if ((mainwindow->zoomMode())){
    //zooming in 1 degree of freedom (energy)
      QPoint topleft(startpos.x(),r.top());
      QPoint bottomright(thispos.x(),r.bottom());
      //QPainter paint( this ); //not needed anymore and causes Qt runtime complaint that paint can only be done in paintevent
      //paint.drawRect(grabrect);//overdraw the old one
      grabrect=QRect(topleft,bottomright);
      //paint.drawRect(grabrect);//draw the new one
      rubberrect->setGeometry(grabrect); //update the rubberect
      }

    if ((mainwindow->selectMode())){
      //selecting only 1 degree of freedom
      QPoint topleft(startpos.x(),r.top());
      QPoint bottomright(thispos.x(),r.bottom());
      //QPainter paint( this );
      //paint.drawRect(grabrect);//overdraw the old one
      grabrect=QRect(topleft,bottomright);
      //paint.drawRect(grabrect);//draw the new one
      rubberrect->setGeometry(grabrect); //update the rubberect
      }
     }
    }
}

void GraphQwt::mouseReleaseEvent(QMouseEvent* e){
  //override the qwidget mouseReleaseEvent
  //check if we are in grabbing mode
  if (grabbing){
     if ((mainwindow->zoomMode())){
     //end of zoom mode
      const int leftindex=convert_coords_to_index(grabrect.left());
      int rightindex=convert_coords_to_index(grabrect.right());
      if (rightindex==leftindex){
	  grabbing=false;
	  rubberrect->hide();
	  //get the normal cursor again
	  QCursor arrow(Qt::ArrowCursor);
	  this->setCursor (arrow);
	  this->repaint();
       	  return;
      }

      //carefull, read out leftindex and rightindex before touching setstartzoomindex and setendzoomindex
      //because they change the result you would get from convert_coords_to_index()
      if (grabrect.left()<grabrect.right()){

        setstartzoomindex(leftindex);
        setendzoomindex(rightindex);
        }
      if (grabrect.left()>grabrect.right()){
        setstartzoomindex(rightindex);
        setendzoomindex(leftindex);
        }
      if (grabrect.left()==grabrect.right()){
        setstartzoomindex(0);
        setendzoomindex(npoints-1);
        }
     //rescale taking into account the new limits
     this->scale();
     }

     if ((mainwindow->selectMode())){
     //end of selection
     //get the grabrect coordinates
     //and convert them to spectrum indices
      setselection(true);
      if (grabrect.left()<grabrect.right()){
        setstartindex(convert_coords_to_index(grabrect.left()));
        setendindex(convert_coords_to_index(grabrect.right()));
        }
      if (grabrect.left()>grabrect.right()){
        setstartindex(convert_coords_to_index(grabrect.right()));
        setendindex(convert_coords_to_index(grabrect.left()));
        }
      if (grabrect.left()==grabrect.right()){
        setstartindex(0);
        setendindex(0);
        }
      }
    }
  //grabbing mode ends here
  grabbing=false;
  rubberrect->hide();
  //get the normal cursor again
  QCursor arrow(Qt::ArrowCursor);
  this->setCursor (arrow);
  this->repaint();
}

int GraphQwt::convert_coords_to_index(int i){
  //convert image coordinates to indices in the spectrum
  //take care of scaling etc
  //make sure you return valid indices
  //rectangle r contains the whole spectrum (if not zoomed)
  //these coordinates need to be mapped on the indices between 0 and npoints-1
  int mincoord=r.left();
  int maxcoord=r.right();
  int length=maxcoord-mincoord;
  //check if our coordinate i lies in the range
  if ((i<mincoord)||(i>maxcoord)) return 0;
  const double npoints_zoom=1+getendzoomindex()-getstartzoomindex();
  double dindex=(double(i-mincoord)/double(length))*double(npoints_zoom);
  int index=int(dindex)+getstartzoomindex(); //round it off to integer
  //check if this index is valid
  if (index<0) return 0;
  if (index>(npoints-1)) return npoints-1;
  return index;
}

int GraphQwt::convert_index_to_coords(int i){
  //convert spectrum index to image coordinates
  //take care of scaling etc
  //rectangle r contains the whole spectrum (if not zoomed)
  //these coordinates need to be mapped on the indices between 0 and npoints-1
  int mincoord=r.left();
  int maxcoord=r.right();
  int length=maxcoord-mincoord;
  //check if index i is valid
  if ((i<0)||(i>(npoints))) return 0;
  const double npoints_zoom=1+getendzoomindex()-getstartzoomindex();
  double dcoord=double(length)*(double(i-getstartzoomindex())/double(npoints_zoom));
  int coord=mincoord+int(dcoord); //round it off to integer
  //check if coordinate is valid
  if (coord<mincoord) return mincoord;
  if (coord>maxcoord) return maxcoord;
  return coord;
}

void GraphQwt::resetselection(){
  grabrect=QRect(0,0,0,0);
  setselection(false);
  setstartindex(0);
  setendindex(0);
  repaint();
}

size_t GraphQwt::getstartzoomindex(int layer)const{
    if (data[layer].size()!=data[0].size()){
        for (size_t i=0;i<data[layer].size();i++){
            if (data[layer][i].x()>data[0][startzoomindex].x()) return i; //for different size, take layer 0 as reference
        }
        return 0;
    }
    return startzoomindex;
}
size_t GraphQwt::getendzoomindex(int layer)const{
     if (data[layer].size()!=data[0].size()){
        for (size_t i=0;i<data[layer].size();i++){
            if (data[layer][i].x()>data[0][endzoomindex].x()) return i; //for different size, take layer 0 as reference
        }
        return data[layer].size();
    }
    return endzoomindex;
}
void GraphQwt::setstartindex(int i){
    if ((i>=0)&&(i<npoints)) startindex=i;
}
void GraphQwt::setendindex(int i){
     if ((i>=0)&&(i<npoints)) endindex=i;
}
void GraphQwt::setstartzoomindex(int i){
     if ((i>=0)&&(i<npoints)) startzoomindex=i;
}
void GraphQwt::setendzoomindex(int i){
     if ((i>=0)&&(i<npoints)) endzoomindex=i;
}
/*QPointF QwtSeriesData<QPointF>::sample( size_t i ) const{
    //return a qpointf at index i
    const QPointF thispoint(spectrumptr->getenergy(i),spectrumptr->getvalue(i));
    return thispoint;
}
size_t QwtSeriesData<QPointF>::size() const{
    return spectrumptr->getnpoints();
}
QRectF QwtSeriesData<QPointF>::boundingRect() const{
    const QRectF myrect(left,top,width,height)
    return thispoint;
}
*/
