/***************************************************************************
                          graph.cpp  -  class to draw 2D graphs
                             -------------------
    begin                : Sat Oct 12 2002
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

//#define GRAPH_DEBUG



#include "src/gui/graph.h"
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
#include <qwt_scale_engine.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_canvas.h>
#include <qwt_plot_zoomer.h>
#include <qwt_plot_panner.h>
#include <qwt_plot_renderer.h>
#include <qwt_picker_machine.h>

//#define GRAPH_DEBUG


extern QWorkspace* getworkspaceptr();
extern MenuEelsmodel* getmenuptr();
//
// Graph - a widget that draws a Graph.
//
class QwtPlotCurveSpecial: public QwtPlotCurve
{
private:
    bool penvectorset;
    QVector<QPen> v_pen;
public:
    QwtPlotCurveSpecial(const QString &title=QString::null):
        QwtPlotCurve(title)
    {penvectorset=false;};
    void setPenVector(QVector<QPen> &vec)
    {
        v_pen = vec;
        penvectorset=true;
    };
    bool hasPenVector(){
      return penvectorset;
    };
    void drawlines(QPainter *p, const QwtScaleMap &xMap, const QwtScaleMap &yMap, const QRectF &canvasRect, int from, int to) const{
        if (penvectorset){
                //overload drawlines
                //change color of line if points are excluded
               for (int counter = from; counter < to; counter++)
               {
                p->setPen(v_pen[counter]);
                QwtPlotCurve::drawLines(p,xMap,yMap,canvasRect,counter,counter+1);
               }
        }
        else{
             QwtPlotCurve::drawLines(p,xMap,yMap,canvasRect,from,to);
        }

     };
};
class Zoomer: public QwtPlotZoomer
{
public:
    Zoomer( int xAxis, int yAxis, QWidget *canvas ):
        QwtPlotZoomer( xAxis, yAxis, canvas )
    {
        setTrackerMode( QwtPicker::AlwaysOff );
        setRubberBand( QwtPicker::NoRubberBand );

        // RightButton: zoom out by 1
        // Ctrl+RightButton: zoom out to full size

        setMousePattern( QwtEventPattern::MouseSelect2,
            Qt::RightButton, Qt::ControlModifier );
        setMousePattern( QwtEventPattern::MouseSelect3,
            Qt::RightButton );
    }
};
//
// Constructs a Graph widget.
//

Graph::Graph( QWorkspace *parent, const char *name,Spectrum *spec)
    :QwtPlot(parent),qwtdata(1, QVector<QPointF>(spec->getnpoints()))
{
    multispectrum=false;
    spectrumptr=spec;
    this->setWindowTitle(name);
    parent->addWindow(this); //add it explicitly to the workspace (important in QT4)
    #ifdef GRAPH_DEBUG
    std::cout << "Constructor of graph\n";
    std::cout << "parent adres is "<<parent<<"\n";
    std::cout << "spectrum adres is: "<<spec<<"\n";
    std::cout << "The size of data is :"<<data.size()<<"\n";
    std::cout << "The size of data[0] is:"<<data[0].size()<<"\n";
    #endif
  Init();
    #ifdef GRAPH_DEBUG
    std::cout << "End of constructor of Graph\n";
    #endif
}



Graph::Graph( QWorkspace *parent, const char *name,Multispectrum *mspec)
   : QwtPlot(parent),qwtdata(mspec->getsize(), QVector<QPointF>(mspec->getnpoints()))
{

    this->setWindowTitle(name);
    parent->addWindow(this); //add it explicitly to the workspace
      #ifdef GRAPH_DEBUG
    std::cout << "Constructor of graph for multispectrum\n";
    std::cout << "parent adres is "<<parent<<"\n";
    std::cout << "multispectrum adres is: "<<mspec<<"\n";
    std::cout << "The size of data is :"<<data.size()<<"\n";
    std::cout << "The size of data[0] is:"<<data[0].size()<<"\n";
    #endif


    multispectrum=true; //it's a multispectrum
    spectrumptr=mspec->getcurrentspectrum(); //a pointer to the current active spectrum
    multispectrumptr=mspec; //a pointer to the whole multispectrum

  //prepare first spectrum
  Init();

}


Graph::~Graph(){
//can't delete spectrum since spectrum deletes its graph when closed
//this gives infinite recursion
       if (rubberrect!=0){
          delete(rubberrect);
       }
       //the d_curves are automagically deleted by the destructor this as well
}

void Graph::Init(){
     #ifdef GRAPH_DEBUG
    std::cout << "Start of init\n";
    #endif


    getmainwindowptr(); //get a pointer to the main window
    // setFrameStyle( QFrame::Raised);
     setLineWidth( 2 );
     setMidLineWidth( 3 );
     npoints=spectrumptr->getnpoints();
    //prepare first single spectrum plot
    //you can add spectra later with addgraph



    nplots=1;
    //resize data field for containing the graph
    qwtdata.clear(); //start with empty data
    qwtdata.resize(1);
    qwtdata[0].resize(npoints);
    penvector.resize(npoints);
    QwtPlotCurveSpecial * mycurveptr=new QwtPlotCurveSpecial("plot");

    mycurveptr->setPen(normalcolor);
    mycurveptr->setStyle( QwtPlotCurve::Lines );
    mycurveptr->setRenderHint( QwtPlotItem::RenderAntialiased );


    d_curves.push_back(mycurveptr);
    mycurveptr->attach(this);

    copydata(0,spectrumptr); //copy the spectrum data into the graph data field (for multispec we take the current spectrum)

    setdefaults(); //set default border styles and colors etc
    getmainwindowptr(); //get a pointer to the main window

    xlabel="energy [eV]";
    ylabel="e- counts";
    setAxisTitle( QwtPlot::xBottom, xlabel );
    setAxisTitle( QwtPlot::yLeft, ylabel );
    setAxisMaxMajor( QwtPlot::xBottom, 10 );
    setAxisMaxMinor( QwtPlot::xBottom, 9 );

    QwtLinearScaleEngine* myscaleenginex=new QwtLinearScaleEngine;
    myscaleenginex->setMargins	(0,0);
    myscaleenginex->setAttribute(QwtLinearScaleEngine::Floating);
    setAxisScaleEngine( QwtPlot::xBottom, myscaleenginex);
    QwtLinearScaleEngine* myscaleenginey=new QwtLinearScaleEngine;
    myscaleenginey->setMargins	(0,0);
    myscaleenginey->setAttribute(QwtLinearScaleEngine::Floating);
    setAxisScaleEngine( QwtPlot::yLeft, myscaleenginey);

    show();

    //init the zoom and pan function
    //QwtPlot::yLeft
    d_zoomer[0] = new Zoomer( QwtPlot::xBottom, QwtPlot::yLeft,this->canvas() );
    d_zoomer[0]->setRubberBand( QwtPicker::RectRubberBand );
    d_zoomer[0]->setRubberBandPen( QColor( Qt::green ) );
    d_zoomer[0]->setTrackerMode( QwtPicker::AlwaysOff );
    d_zoomer[0]->setTrackerPen( QColor( Qt::black ) );
    d_zoomer[1] = new Zoomer( QwtPlot::xTop, QwtPlot::yRight,this->canvas() );
    d_panner = new QwtPlotPanner( this->canvas() );
    d_panner->setMouseButton( Qt::MidButton );


    d_picker[0] = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOff,this->canvas() );
    //d_picker[0] = new QwtPlotPicker( QwtPlot::xBottom, QwtPlot::yLeft,QwtPlotPicker::VLineRubberBand, QwtPicker::AlwaysOff,this->canvas() );
    //QwtPickerDragPointMachine
   d_picker[0]->setStateMachine( new QwtPickerDragRectMachine() );
  // d_picker[0]->setStateMachine( new QwtPickerDragLineMachine() );
    d_picker[0]->setRubberBandPen( QColor( Qt::green ) );
    d_picker[0]->setRubberBand( QwtPicker::RectRubberBand );
    //d_picker[0]->setRubberBand( QwtPlotPicker::VLineRubberBand );
    d_picker[0]->setTrackerPen( QColor( Qt::black ) );
    d_picker[1] = new QwtPlotPicker( QwtPlot::xTop, QwtPlot::yRight, QwtPlotPicker::RectRubberBand, QwtPicker::AlwaysOff,this->canvas() );
    //d_picker[1] = new QwtPlotPicker( QwtPlot::xTop, QwtPlot::yRight, QwtPlotPicker::VLineRubberBand, QwtPicker::AlwaysOn,this->canvas() );

    connect(d_picker[0], SIGNAL(selected(const QPolygon &)), SLOT(selectionmade(const QPolygon &)));

    this->autoReplot();
}

void Graph::selectionmade(const QPolygon & 	polygon){
    //get the selection points and apply to spectrum
    QRect r=polygon.boundingRect();
    double estart= this->invTransform(QwtPlot::xBottom,r.left());
    double estop=this->invTransform(QwtPlot::xBottom,r.right());
    setstartindex(spectrumptr->getenergyindex(estart));
    setendindex(spectrumptr->getenergyindex(estop));
    setselection(true);

}
void Graph::setdefaults(){
      #ifdef GRAPH_DEBUG
    std::cout << "Start of setdefaults\n";
    #endif
// min size for this widget
 this->setMinimumSize(300,150);

   #ifdef GRAPH_DEBUG
    std::cout << "Before reseting grabbing and area selection\n";
    #endif
//reset all grabbing and arrea selection
  grabbing=false;
  grabrect=QRect(0,0,0,0);
  setselection(false);
  setstartindex(0);
  setendindex(npoints);
  rubberrect=0;
   #ifdef GRAPH_DEBUG
    std::cout << "Before reseting zoom\n";
       std::cout << "npoints="<<npoints<<"\n";

    #endif
//reset zoom
  setstartzoomindex(0);
  setendzoomindex(npoints-1);
  #ifdef GRAPH_DEBUG
  std::cout << "endzoomindex="<<endzoomindex<<"\n";
  std::cout << "startzoomindex="<<startzoomindex<<"\n";
    std::cout << "before defining colors\n";
    #endif
//define default colors
  setnormalcolor(QColor("blue"));
  setexcludecolor(QColor("cyan")); //a darker version of normalcolor
  setaxiscolor(QColor("black"));
  setbgcolor(QColor("white"));
  #ifdef GRAPH_DEBUG
    std::cout << "Before mycolors\n";
    #endif
  mycolors.clear();
  mycolors.push_back(QColor("red"));
  mycolors.push_back(normalcolor);
  mycolors.push_back(QColor("green"));
  dotsize=10; //size of a dot in the plot with style=2 in pixels
  #ifdef GRAPH_DEBUG
    std::cout << "End of setdefaults\n";
    #endif
}

void Graph::getmainwindowptr(){
      #ifdef GRAPH_DEBUG
    std::cout << "Start of getmainwindowptr\n";
    #endif
  //get a pointer to the main window to make possible to ask
  //if we are in zoom or exclude regions mode
  try{
    mainwindow=getmenuptr();//make sure to enable RTTI in BCC32 otherwise an error about polymorphism is generated
    }
  catch(...){
    Saysomething mysay(0,"error","Something went wrong when trying to get the mainwidget in graph");
    throw;
    }
}




void Graph::copydata(int layer,Spectrum* spec){

 for (size_t i=0;i<spec->getnpoints();i++){
     const double ydata=spec->getcounts(i);
     const double xdata=spec->getenergy(i);
     //and copy in the qwt specific store
     (qwtdata[layer][i]).setX(xdata);
     (qwtdata[layer][i]).setY(ydata);
     if ((spec->isexcluded(i))&&(layer==0)){
        penvector[i]=excludecolor;
     }
     else{
        penvector[i]=normalcolor;
     }
  }
 //and attach it to a curve
 d_curves[layer]->setSamples(qwtdata[layer]);
if (layer==0){
 d_curves[layer]->setPenVector(penvector);
}

#ifdef GRAPH_DEBUG
 std::cout <<"end of copydata\n";
#endif
}


void Graph::reinit(){
//reinitialise this graph to a new spectrum in the multispectrum
//used when a multispectrum changes its current spectrum
    if (ismultispectrum()){
        copydata(0,multispectrumptr->getcurrentspectrum()); //copy the new data into layer 0, other layers are not affected
    }
 repaint();
}

void Graph::addgraph(Spectrum *spec)
{
  nplots++;
  qwtdata.resize(nplots);    //increase size of data vector
  qwtdata[nplots-1].resize(spec->getnpoints());
  QwtPlotCurveSpecial * mycurveptr=new QwtPlotCurveSpecial("plot");
  mycurveptr->setPen( Qt::darkRed );
  mycurveptr->setStyle( QwtPlotCurve::Lines );
  mycurveptr->setRenderHint( QwtPlotItem::RenderAntialiased );


  d_curves.push_back(mycurveptr);
  mycurveptr->attach(this);

  #ifdef GRAPHDEBUG
   std::cout <<"addgraph function\n";
  std::cout <<"the new size of data is "<<data.size()<<"\n";
   std::cout <<"the new size of data[0] is "<<data[0].size()<<"\n";
  #endif
  //add the data in the new layer
   copydata(nplots-1,spec);
   this->replot();
}
void Graph::removelastgraph(){
    nplots--;
  qwtdata.resize(nplots);    //decrease size of data vector

  QwtPlotCurveSpecial * mycurveptr=d_curves.last();
  d_curves.pop_back();
  mycurveptr->detach();
  delete(mycurveptr);


  #ifdef GRAPHDEBUG
   std::cout <<"removelastgraph function\n";
  std::cout <<"the new size of data is "<<data.size()<<"\n";
   std::cout <<"the new size of data[0] is "<<data[0].size()<<"\n";
  #endif
}


void Graph::updategraph(int layer,Spectrum* spec){
    //check if valid layer
    if  (!this->validlayer(layer)) return;
    //add the data
    copydata(layer,spec);
    repaint();
}
bool Graph::validlayer(int layer)const{
return ((layer>=0)&&(layer<nplots));
}

void Graph::setxlabel(const char* xl){xlabel=xl;setAxisTitle( QwtPlot::xBottom, xlabel );}

void Graph::setylabel(const char* yl){ylabel=yl;setAxisTitle( QwtPlot::yLeft, ylabel );}

void Graph::updateCaption(){}

void Graph::paintEvent( QPaintEvent *event )
{
    replot();
}

void Graph::mouseMoveEvent(QMouseEvent *evt)
        {
    //do nothing we don't want moves
}

Spectrum* Graph::getspectrumptr(){
  return spectrumptr;
}
Multispectrum* Graph::getmultispectrumptr(){
  if (multispectrum) return multispectrumptr;
  else return 0;
}

void Graph::mousePressEvent(QMouseEvent* e){
    //update the state of the zoom or selection
    d_panner->setEnabled( mainwindow->zoomMode());
    d_zoomer[0]->setEnabled( mainwindow->zoomMode() );
    d_zoomer[0]->zoom( 0 );
    d_zoomer[1]->setEnabled( mainwindow->zoomMode() );
    d_zoomer[1]->zoom( 0 );
    d_picker[0]->setEnabled( mainwindow->selectMode());
    d_picker[1]->setEnabled( mainwindow->selectMode());
}
/*
void Graph::mousePressEvent(QMouseEvent* e){
  //override the qwidget mousepressEvent
  //if the left mouse button was pressed
  if (e->button()==Qt::LeftButton){
    startpos=e->pos();
    //check if we are in Zoom mode
    if ((mainwindow->zoomMode())||(mainwindow->selectMode())){
      //see if mouse is really in the graph rectangle
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
      this->repaint();
    }
  }


}

void Graph::mouseMoveEvent(QMouseEvent* e){
  //override the qwidget mouseMoveEvent
  QPoint thispos=e->pos();
  //check if we are in grabbing mode
  if (grabbing){
     setselection(false);//we are making a new selection, the old one is obsolete
    //start dragging a rectangle
    //see if mouse is in the graph rectangle
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
*/
void Graph::mouseReleaseEvent(QMouseEvent* e){
  //override the qwidget mouseReleaseEvent
  //check if we are in grabbing mode
    if (mainwindow->selectMode()){
     //   const QPolygon p1 =d_picker[0]->pickedPoints();
      //  const QPolygon p2 =d_picker[1]->pickedPoints();
        //and convert these to energies to select or deselect

    }

    /*
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
  */

}



void Graph::resetselection(){
  grabrect=QRect(0,0,0,0);
  setselection(false);
  setstartindex(0);
  setendindex(0);
  repaint();
}


void Graph::setstartindex(int i){
    if ((i>=0)&&(i<npoints)) startindex=i;
}
void Graph::setendindex(int i){
     if ((i>=0)&&(i<npoints)) endindex=i;
}
void Graph::setstartzoomindex(int i){
     if ((i>=0)&&(i<npoints)) startzoomindex=i;
}
void Graph::setendzoomindex(int i){
     if ((i>=0)&&(i<npoints)) endzoomindex=i;
}
void Graph::setstyle(size_t layer,size_t style){
    if (validlayer(layer)){
        switch (style){
            case 0: d_curves[layer]->setStyle( QwtPlotCurve::Lines );
                    break;
            case 1:d_curves[layer]->setStyle( QwtPlotCurve::Lines );
                    break;
            case 2:d_curves[layer]->setStyle( QwtPlotCurve::Dots);
                    break;
            case 3:d_curves[layer]->setStyle( QwtPlotCurve::Sticks);
                break;
            default:d_curves[layer]->setStyle( QwtPlotCurve::Lines );
        }
    }
}
