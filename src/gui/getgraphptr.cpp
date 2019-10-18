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
 * eelsmodel - gui/getgraphptr.cpp
 **/

#include "src/gui/getgraphptr.h"

//#define GETGRAPHDEBUG

#include <iostream>

#include <QApplication>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QWidget>
//#include <QMdiArea>
#include <QMdiArea>
#include "src/core/eelsmodel.h"

#include "src/gui/graph.h"
#include "src/gui/saysomething.h"

QMdiArea* getworkspaceptr();

Getgraphptr::Getgraphptr(QWidget *parent, const char *name,const char* message )
: QDialog(parent,Qt::WindowStaysOnTopHint)
{
    //was false,Qt::WStyle_StaysOnTop
    this->setWindowTitle(name);
    specptr=0;
    //show the message and an OK and cancel button
    this->setFocusPolicy(Qt::NoFocus);     
    QGridLayout *vbox = new QGridLayout( this );

    QLabel* mylabel=new QLabel(message,this);
    vbox->addWidget(mylabel,0,0,1,2);
    
    QPushButton *OK = new QPushButton( "&OK", this);
    QPushButton *cancel = new QPushButton( "&Cancel", this);
    vbox->addWidget(OK,1,0);
    vbox->addWidget(cancel,1,2);

    //wait for a user to click either OK or cancel
    connect( OK, SIGNAL( clicked()), this, SLOT( slot_ok() ) );
    connect( cancel, SIGNAL( clicked()), this, SLOT( slot_cancel() ) );
}

Getgraphptr::~Getgraphptr(){
}

void Getgraphptr::slot_ok(){
	#ifdef GETGRAPHDEBUG
	std::cout <<"Getgraphptr::Ok pressed\n";
	#endif

	Graph* topgraph=0;
	multi=false;	
	specptr=0;
	mspecptr=0;
	//get a list of windows in the stacking order
	//last one is topmost
        const QMdiArea* myworkspace=getworkspaceptr();
        QList<QMdiSubWindow *> wlist=myworkspace->subWindowList(QMdiArea::StackingOrder);
	//start from the last one until a graph is encountered	
	for (size_t i=0; i<wlist.size(); i++){		
		//check if the item is a graph
		 #ifdef GETGRAPHDEBUG
	     			std::cout <<"wlist.size="<<wlist.size()<<"\n";
	     #endif	   
	     try{	     	     		
	     		//get the active window and try to convert to a graph
	    	 	QWidget* mywindow=wlist[wlist.size()-i-1];
				#ifdef GETGRAPHDEBUG
	     			std::cout <<"made mywindow\n";
	     			std::cout <<"name="<<(mywindow->windowTitle()).toStdString() <<"\n" ;
	     		#endif	 
	     			
	       		topgraph=dynamic_cast<Graph*>(mywindow);
	     		#ifdef GETGRAPHDEBUG
	     			std::cout <<"topgraph"<<topgraph<<"\n";
	     		#endif	     	
	     }
	     catch(...){
	    	 	//something went wrong
	     		#ifdef GETGRAPHDEBUG
	     			std::cout <<"catch in topgraph:"<<topgraph<<"\n";
	     		#endif
	     		topgraph=0;
	     }
	     if (topgraph!=0){
	    	 	//we seem to have a graph, now get the spectrum pointer of this graph
	    	 	//but beware of errors
	     		#ifdef GETGRAPHDEBUG
	     			std::cout <<"topgraph!=0 topgraph:"<<topgraph<<"\n";
	     		#endif
	     		if (topgraph->ismultispectrum()){
	     			multi=true;
	     			try{
	           			mspecptr=topgraph->getmultispectrumptr();
	     			}
	     			catch(...){
	     				//didn`t work
	     				multi=false;
	     				specptr=0;
	     				mspecptr=0;
	     			}
	     			break; //we are ready going through the window list
	     		}
	     		else{
	     			multi=false;
	     			try{
	           			specptr=topgraph->getspectrumptr();
	     			}
	     			catch(...){
	     				multi=false;
	     				specptr=0;
	     				mspecptr=0;
	     			}
	     			break; //we are ready going through the window list
	     		}
	     	}	     	
	}	 			
	emit(accept());
}

void Getgraphptr::slot_cancel(){
	multi=false;
	specptr=0;
	mspecptr=0;
	emit(accept());
}

Spectrum* Getgraphptr::getpointer(){
	return specptr;
}

Multispectrum* Getgraphptr::getmultipointer(){
	return mspecptr;
}

bool Getgraphptr::ismulti(){
	return multi;
}
