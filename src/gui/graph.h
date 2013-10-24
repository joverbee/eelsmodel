/***************************************************************************
                          graph.h  -  description
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
/**
  *@author Jo Verbeeck
  */

#ifndef GRAPH_H
#define GRAPH_H
#include <qwidget.h>
#include <qmatrix.h>
#include <QMouseEvent>
#include <QPaintEvent>
#include <vector>
#include <qpainter.h>
#include <QRubberBand>
#include <qwt_plot.h>

class Spectrum;
class Multispectrum;
class QWorkspace;
class MenuEelsmodel;

  class Graph : public QWidget
{
  Q_OBJECT
  public:
    Graph( QWorkspace *parent=0, const char *name=0,Spectrum *spec=0); //a graph of a normal spectrum
    Graph( QWorkspace *parent, const char *name,Multispectrum *mspec); //a graph of a multispectrum
    ~Graph();
    void reinit();
    void Init();
    void addgraph(Spectrum* spec);
    void removelastgraph();
    void setxlabel(const char* xl);
    void setylabel(const char* yl);
    void updategraph(int layer,Spectrum* spec);
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
    void setstyle(int layer,size_t style);
    void setcaption(std::string name){this->setWindowTitle(name.c_str());} //change caption
protected:
    void paintEvent( QPaintEvent * );
    void scale();
    void mousePressEvent(QMouseEvent* e);//override the qwidget mousepressEvent
    void mouseMoveEvent(QMouseEvent* e);//override the qwidget mouseMoveEvent
    void mouseReleaseEvent(QMouseEvent* e);//override the qwidget mouseReleaseEvent
    void setselection(bool b){selection=b;}
    void setstartindex(int i);
    void setendindex(int i);
    void setstartzoomindex(int i);
    void setendzoomindex(int i);
    void setzoom(bool b){zoomed=b;}

    int convert_coords_to_index(int i);
    int convert_index_to_coords(int i);
    void setexcludecolor(QColor c){excludecolor=c;}
    void setnormalcolor(QColor c){normalcolor=c;}
    void setaxiscolor(QColor c){axiscolor=c;}
    void setbgcolor(QColor c){bgcolor=c;}
    void setdefaults();
    void getmainwindowptr();
    void copydata(int layer,Spectrum* spec);

private slots:
    void        updateCaption();
    void slot_graph_clicked();
private:
    int npoints;      //number of data points
    int nplots;       //number of layers in the plot
    double scalefactor;
    //    std::vector <QPoint> singledata; //contains one single graph of pixel points
    std::vector < std::vector<QPoint> > data; //contains a set of graphs for a multi-graph plot
    QVector< QPointF >	qwtdata;
    QwtPlot * myPlot; //a pointer to the qwt plot window
    QTransform myworld; //scale conversion matrix real positions- window positions
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
    std::vector<size_t> stylelist; //a list with plot style code per layer
};

#endif








