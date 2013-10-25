/***************************************************************************
                          drudeoptions.cpp  -  description
                             -------------------
    begin                : Sat Nov 23 2002
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

#include "src/gui/Drudeoptions.h"

#include <QComboBox>
#include <QCheckBox>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QPushButton>
#include <QString>
#include <QStringList>
#include <QValidator>
#include <QVBoxLayout>
#include <QWorkspace>

Drudeoptions::Drudeoptions(QWorkspace *parent, const char *name,size_t nosc,size_t thetapoints,bool integrate,bool show,bool dokroeger,bool dorel,int outputformat) : QDialog(parent) {
 parent->addWindow(this); //add it explicitly to the workspace


QVBoxLayout* bgrp = new QVBoxLayout(this);

//a radio button to swith sumrule

kroegerbutton =new QCheckBox ("Apply Kroeger formula",parent);
bgrp->addWidget(kroegerbutton);
kroegerbutton->setChecked(dokroeger);

relbutton =new QCheckBox ("Apply Relativistic correction",parent);
bgrp->addWidget(relbutton);
relbutton->setChecked(dorel);


intbutton =new QCheckBox ("Integrate Kroeger formula in energy",parent);
bgrp->addWidget(intbutton);
intbutton->setEnabled(dokroeger);
intbutton->setChecked(integrate);


showbutton =new QCheckBox ("Show q,E plane",parent);
bgrp->addWidget(showbutton);
showbutton->setEnabled(false);
showbutton->setChecked(false);


//nr of points
valstring="%1"; //required by MacOSX otherwise empty text window
int fieldwidth=-1;
    char format='d';
    int precision=0;
    int_validator=new QIntValidator(this);
    nmaxedit=new QLineEdit(valstring.arg ((double)thetapoints,fieldwidth,format,precision ));
    QLabel* nmaxlabel=new QLabel("thetasteps");
    (void) nmaxlabel;//get rid of unused variable warning
    nmaxedit->setValidator (int_validator);  //make sure only ints are entered
    nmaxedit->setEnabled(dokroeger);
    bgrp->addWidget(nmaxlabel);
    bgrp->addWidget(nmaxedit);

    oscedit=new QLineEdit(valstring.arg ((double)nosc,fieldwidth,format,precision ));
    QLabel* osclabel=new QLabel("nr. of points");
    (void) osclabel;//get rid of unused variable warning
    oscedit->setValidator (int_validator);  //make sure only ints are entered
    bgrp->addWidget(osclabel);
    bgrp->addWidget(oscedit);

//choice of output graphs

mylist=new QStringList;
QString s1="Eps1,Eps2";
mylist->append( s1 );
QString s2="Re(1/eps),Im(1/eps)";
mylist->append( s2 );
QString s3= "n,K";
mylist->append( s3 );
mylistw=new QComboBox ( this);
mylistw->addItems (*mylist);
mylistw->setCurrentIndex(outputformat);
bgrp->addWidget(mylistw);

//OK and cancel buttons
//OK button

     QPushButton *OK = new QPushButton( "&OK");
     QPushButton *Cancel = new QPushButton( "&Cancel");
     bgrp->addWidget(OK);
     bgrp->addWidget(Cancel);
     connect( OK, SIGNAL( clicked()), this, SLOT( slot_OK() ) );
     connect( Cancel, SIGNAL( clicked()), this, SLOT( slot_Cancel() ) );

     //connect to the checkboxes
    connect( kroegerbutton, SIGNAL( toggled(bool)), this, SLOT( slot_dokroeger(bool) ) );
    connect( relbutton, SIGNAL( toggled(bool)), this, SLOT( slot_dorel(bool) ) );


this->show();
 this->setMinimumSize(this->minimumSizeHint()); //needed since QT4 to make sure the widget is not shown minimized
}
Drudeoptions::~Drudeoptions(){
}


void Drudeoptions::slot_OK(){
    emit accept();
}
void Drudeoptions::slot_Cancel(){
    //gather results when OK
    emit reject();
}

size_t Drudeoptions::getthetapoints(){
    return size_t(getnumber(nmaxedit->text()));
}
size_t Drudeoptions::getnosc(){
    return size_t(getnumber(oscedit->text()));
}

bool Drudeoptions::getintegrate(){
    return intbutton->isChecked();
}
bool Drudeoptions::getshow(){

    return showbutton->isChecked();
}
bool Drudeoptions::getdokroeger(){
    return kroegerbutton->isChecked();
}

bool Drudeoptions::getdorel(){
    return relbutton->isChecked();
}

int Drudeoptions::getoutputformat(){
    //returns index or -1 if no item is selected
    return mylistw->currentIndex ();
}

double Drudeoptions::getnumber(const QString& valstring)const{
  bool ok;
  double result=valstring.toDouble(&ok);
  if (ok) return result;
  else return 0.0;
}

void Drudeoptions::slot_dokroeger(bool b){
    showbutton->setEnabled(false);
    nmaxedit->setEnabled(b);
    intbutton->setEnabled(b);
}

void Drudeoptions::slot_dorel(bool b){
    intbutton->setEnabled(false);
    showbutton->setEnabled(false);
    nmaxedit->setEnabled(false);
}
