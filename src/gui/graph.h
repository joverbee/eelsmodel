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
 * eelsmodel - gui/graph.h
 **/

#ifndef GRAPH_H
#define GRAPH_H

#include <vector>

#include <QMatrix>
#include <QMouseEvent>
#include <QPaintEvent>
#include <QPainter>
#include <QRubberBand>

#include <qwt_plot.h>
#include <qwt_plot_curve.h>

class QPolygon;
class QMdiArea;

class QwtPlotPanner;
class QwtPlotMarker;
class QwtPlotCurveSpecial;

class MenuEelsmodel;
class Multispectrum;
class Picker;
class Spectrum;
class Zoomer;

class Graph : public QwtPlot
{
  Q_OBJECT
  public:
    Graph( QMdiArea *parent=0, const char *name=0,Spectrum *spec=0); //a graph of a normal spectrum
    Graph( QMdiArea *parent=0, const char *name=0,Multispectrum *mspec=0); //a graph of a multispectrum
    ~Graph();
    void reinit();
    void Init();
    void addgraph(Spectrum* spec);
    void removelastgraph();
    void setxlabel(const char* xl);
    void setylabel(const char* yl);
    bool validlayer(int layer)const;
    Spectrum* getspectrumptr();
    Multispectrum* getmultispectrumptr();
    bool getselected()const{return selection;}
    bool iszoomed()const{return zoomed;}
    bool ismultispectrum()const{return multispectrum;}
    int getstartindex()const{return startindex;}
    int getendindex()const{return endindex;}
    size_t getstartzoomindex(int layer=0)const;
    size_t getendzoomindex(int layer=0)const;
    int getnplots()const{return nplots;}
    void resetselection();
    void setstyle(size_t layer,size_t style);
    void setcaption(std::string name){this->setWindowTitle(name.c_str());} //change caption
    void updategraph(int layer,Spectrum* spec);
protected:
    void paintEvent( QPaintEvent * );
    void mousePressEvent(QMouseEvent* e);//override the qwidget mousepressEvent
   // void mouseMoveEvent(QMouseEvent* e);//override the qwidget mouseMoveEvent
    //void mouseReleaseEvent(QMouseEvent* e);//override the qwidget mouseReleaseEvent
    void mouseMoveEvent(QMouseEvent *evt);
    void setselection(bool b){selection=b;}
    void setstartindex(int i);
    void setendindex(int i);
    void setstartzoomindex(int i);
    void setendzoomindex(int i);
    void setzoom(bool b){zoomed=b;}
    void setexcludecolor(QColor c){excludecolor=c;}
    void setnormalcolor(QColor c){normalcolor=c;}
    void setaxiscolor(QColor c){axiscolor=c;}
    void setbgcolor(QColor c){bgcolor=c;}
    void setdefaults();
    void getmainwindowptr();
    void copydata(int layer,Spectrum* spec);

private slots:
    void        updateCaption();
    void        selectionmade(const QPolygon & 	polygon);
   //void slot_graph_clicked();
private:
    int npoints;      //number of data points
    int nplots;       //number of layers in the plot
    QVector<QwtPlotCurveSpecial*> d_curves;
    QVector<QVector< QPointF > >	qwtdata;
    QVector<QVector< QPen > >	penvector;
    Zoomer *d_zoomer[2];
    Picker *d_picker[2];
    QwtPlotPanner *d_panner;
    QwtPlotMarker* d_marker1;
    QwtPlotMarker* d_marker2;

    double border;
    double xmax;
    double ymax;
    double xmin;
    double ymin;
    int xtickcount,ytickcount;
    QString xlabel,ylabel;
    double ticksize;
    double fontsize;
    int precision;
    std::vector<QColor> mycolors;
    int dotsize; //size of dots in the plot
    Spectrum* spectrumptr;
    Multispectrum* multispectrumptr;
    MenuEelsmodel* mainwindow; //pointer to the main window
    QRect r; //the rectangle of the graph region
    QRect grabrect;
    QRubberBand* rubberrect;
    //QPainter* paint; //pointer to paint arrea
    QPoint startpos;//position where mouse was first clicked
    QPoint endpos;
    bool grabbing;
    bool selection;
    bool multispectrum;
    int startindex;
    int endindex;
    int startzoomindex;
    int endzoomindex;
    bool zoomed;
    QColor excludecolor,normalcolor,axiscolor,bgcolor;
};

#endif








