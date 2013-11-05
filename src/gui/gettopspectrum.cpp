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
 * eelsmodel - gui/gettopspectrum.cpp
 **/

#include "src/gui/gettopspectrum.h"

#include <QApplication>
#include <QWidget>

#include "src/core/spectrum.h"

#include "src/gui/graph.h"
#include "src/gui/saysomething.h"

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
