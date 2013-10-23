/***************************************************************************
                          gettopspectrum.cpp  -  description
                             -------------------
    begin                : Sat Dec 7 2002
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

#include "src/gui/gettopspectrum.h"
#include "src/gui/saysomething.h"
#include <qapplication.h>
#include <qwidget.h>
#include "src/core/spectrum.h"
#include "src/gui/graph.h"

//look in getgraphptr if you really want to scan through the list of windows
//until you find a graph
//make it also work if the topmost window is this, then select the next window!


Gettopspectrum::Gettopspectrum()  {
}
Gettopspectrum::~Gettopspectrum(){
	
}
Spectrum* Gettopspectrum::getpointer(){
  Spectrum* specptr=0;
  //determine the top window
  QApplication * myapp=qApp;
  //myapp->flush(); //repaint everything so the previous window is active
  //myapp->flushX(); //repaint everything so the previous window is active
  myapp->processEvents();
  QObject* topw=myapp->activeWindow();
  
  Graph* topgraph=0;
  //check if that window is a Graph
  if ((topgraph=(dynamic_cast<Graph*>(topw)))){
  //ask the graph for a pointer to its spectrum
    specptr= topgraph->getspectrumptr();
    }
  else{
    Saysomething mysay(0,"Error","The front window is not a spectrum graph");
    specptr=0;
    }
   return specptr;
}
