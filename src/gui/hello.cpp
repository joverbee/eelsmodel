/***************************************************************************
                          hello.cpp  -  description
                             -------------------
    begin                : Sat Apr 19 2003
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

#include "src/gui/hello.h"

#include <string>

#include <QMessageBox>
#include <QDialog>
#include <QDir>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QPushButton>
#include <QPixmap>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "./welcomescreen/eelsmodel_3_3.xpm"

#include "src/gui/licence.txt"

#ifdef DEBUG
    #include "debug_new.h" //memory leak checker
#endif

Hello::Hello(QWidget *parent, const char *name ) : QDialog(parent,Qt::Dialog) {
this->setWindowModality(Qt::ApplicationModal);
    //QDialog(parent,name,true,0)
//create a nice hello screen in the center of the parent
     // Create Widgets which allow easy layouting
   // vbox = new QVBoxLayout( this );
    //box1 = new QHBoxLayout( this );
    //bgrp2 = new QVBoxLayout(this);

    QGridLayout* lay=new QGridLayout(this);
    lay->setSizeConstraint(QLayout::SetMinimumSize);
     //bgrp2->setMargin(5);
     //bgrp2->setSpacing(2);
     //parentptr=parent;

     //a picture

     dummylabel=new   QLabel("");
     lay->addWidget(dummylabel,0,0,1,2); //a spacer

     const QPixmap helloimg=QPixmap(eelsmodelpix_xpm);
     dummylabel->setPixmap(helloimg);
     //some text
     authorlabel=new   QLabel("Copyright 2003 by Jo Verbeeck");
     lay->addWidget(authorlabel,1,0,1,2);

     emaillabel=new   QLabel("email: eelsmodel@ua.ac.be");
     lay->addWidget(emaillabel,2,0,1,2);


     std::string gnustring="This program is free\n"
     "software;you can redistribute it and/or modify it under the terms of the\n"
     "GNU General Public License as published by the Free Software Foundation\n"
     "either version 2 of the License, or (at your option) any later\n"
     "version. \n\n When publishing results obtained with this software, please cite as: \n J. Verbeeck, S. Van Aert, Ultramicroscopy, Volume 101, 2-4, (2004) 207-224\n";
     gnu=new  QTextEdit(gnustring.c_str());
        lay->addWidget(gnu,3,0,1,2);
     gnu->setReadOnly(true);
     //OK button
     OK = new QPushButton( "OK");
      lay->addWidget(OK,4,0);

     licenceb = new QPushButton( "Licence info");
       lay->addWidget(licenceb,4,1);



     this->setLayout(lay);
     this->show();

     connect( OK, SIGNAL( pressed () ), this, SLOT( slot_ok() ) );
     connect( licenceb, SIGNAL( pressed () ), this, SLOT( slot_licence() ) );
this->exec();



  //  licencewidget=0;
  //  vboxl=0;
  //  box1l=0;
  //  bgrp2l=0;
    gnul=0;
}
Hello::~Hello(){
    //cleanup(); //if I cleanup then crash...why???? supposedly the dialog kills all widgets attached to it
}
void Hello::slot_ok(){
//continue with the program
  emit(done(0)); //also deletes the dialog
}
void Hello::slot_licence(){
  //display the whole GPL in a new widget
    gnul=new  QTextEdit(licence.c_str());
    gnul->setWindowTitle("GNU public licence") ;
    gnul->setReadOnly(true);
    gnul->show();
   emit(done(0)); //also deletes the
}
void Hello::cleanup(){  //clean up all pointers that we newed
    if (licenceb!=0) delete(licenceb);
    if (OK!=0) delete(OK);
    if (gnu!=0) delete(gnu);
    if (emaillabel!=0) delete(emaillabel);
    if (authorlabel!=0) delete(authorlabel);
    if (dummylabel!=0) delete(dummylabel);
    if (bgrp2!=0) delete(bgrp2);
    if (box1!=0) delete(box1);
    if (vbox!=0) delete(vbox);

    //and the licence pointers
    if (gnul!=0) delete(gnul);
    }
