/***************************************************************************
                         getgraphptr.cpp  -  description
                             -------------------
    begin                : Sat Oct 26 2002
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
//#define GETGRAPHDEBUG
#include "src/gui/getgraphptr.h"
#include <qdialog.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <QGridLayout>
#include <iostream>
#include <qlayout.h>
#include "src/gui/saysomething.h"
#include <qworkspace.h>
#include <qwidget.h>
#include  "src/gui/graph.h"
#include <qapplication.h>
#include "src/core/eelsmodel.h"




extern QWorkspace* getworkspaceptr();//main.cpp contains this global function with a pointer to eelsmodel


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
	const QWorkspace* myworkspace=getworkspaceptr();
	QWidgetList wlist=myworkspace->windowList(QWorkspace::StackingOrder);	
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
