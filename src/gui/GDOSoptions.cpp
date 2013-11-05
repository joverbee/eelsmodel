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
 * eelsmodel - gui/GDOSoptions.cpp
 **/

#include "src/gui/GDOSoptions.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QInputDialog>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QString>
#include <QStringList>
#include <QValidator>
#include <QVBoxLayout>
#include <QWorkspace>

#include "src/fitters/fitter.h"

GDosoptions::GDosoptions(QWorkspace *parent, const char *name,size_t degree,size_t interpolationtype,bool dosumrule,size_t broadeningtype,double integrationwidth ,double threshold) : QDialog(parent) {


 parent->addWindow(this); //add it explicitly to the workspace
//a list of interpolation types
QStringList list;
QString s1="Linear";
list.append( s1 );
QString s2= "Cubic spline";
list.append( s2 );
QString s3= "Upsampling";
list.append( s3 );

QVBoxLayout* bgrp = new QVBoxLayout(this);
interpolchooser=new QComboBox (parent);
interpolchooser->setWindowTitle("Interpolation strategy");
interpolchooser->insertItems ( 0,list );

interpolchooser->setCurrentIndex (interpolationtype-1);
bgrp->addWidget(interpolchooser);
connect(interpolchooser, SIGNAL( activated (int)), this, SLOT( slot_changeinterpol(int) ) );
//a list of lifetime broadening
//EGERTON,QUADRATIC,LINEAR,CONSTANT
QStringList list2;
QString s42= "Egerton broadening";
list2.append( s42 );
QString s32= "Quadratic broadening";
list2.append( s32 );
QString s22= "Linear broadening";
list2.append( s22 );
QString s12="Constant broadening";
list2.append( s12 );




lifetimechooser=new QComboBox (parent);
lifetimechooser->setWindowTitle("Interpolation strategy");
lifetimechooser->insertItems(0,list2) ;

lifetimechooser->setCurrentIndex (broadeningtype);
bgrp->addWidget(lifetimechooser);
//only make this active if cubic spline is chosen
if (interpolationtype!=3){
    lifetimechooser->setEnabled(true);
}
else{
    lifetimechooser->setEnabled(false);
}


//a radio button to swith sumrule
sumrulebutton =new QRadioButton ("Enforce Bethe Sum Rule",parent);
bgrp->addWidget(sumrulebutton);
sumrulebutton->setChecked(dosumrule);

//nr of points
valstring="%1"; //required by MacOSX otherwise empty text window
int fieldwidth=-1;
    char format='d';
    int precision=0;
    int_validator=new QIntValidator(this);
    nmaxedit=new QLineEdit(valstring.arg ((double)degree,fieldwidth,format,precision ));
    QLabel* nmaxlabel=new QLabel("nr of points");
    (void) nmaxlabel;//get rid of unused variable warning
    nmaxedit->setValidator (int_validator);  //make sure only ints are entered

    bgrp->addWidget(nmaxlabel);
    bgrp->addWidget(nmaxedit);



//nr of points
//valstring="%1"; //required by MacOSX otherwise empty text window
    fieldwidth=-1;
    format='f';
    precision=2;
    double_validator=new QDoubleValidator(this);
    integrationedit=new QLineEdit(valstring.arg (integrationwidth,fieldwidth,format,precision ));
    QLabel* integrationlabel=new QLabel("Edge onset smoothing [eV]");
    (void) integrationlabel;//get rid of unused variable warning
    integrationedit->setValidator (double_validator);  //make sure only ints are entered

    bgrp->addWidget(integrationlabel);
    bgrp->addWidget(integrationedit);

//treshold level for edge onset
    precision=0;
    thresholdedit=new QLineEdit(valstring.arg (threshold*100.0,fieldwidth,format,precision ));
    QLabel* thresholdlabel=new QLabel("Edge onset threshold [%]");
    (void) thresholdlabel;//get rid of unused variable warning
    thresholdedit->setValidator (double_validator);  //make sure only ints are entered
    bgrp->addWidget(thresholdlabel);
    bgrp->addWidget(thresholdedit);



//OK and cancel buttons
//OK button

     QPushButton *OK = new QPushButton( "&OK");
     QPushButton *Cancel = new QPushButton( "&Cancel");
     bgrp->addWidget(OK);
     bgrp->addWidget(Cancel);
     connect( OK, SIGNAL( clicked()), this, SLOT( slot_OK() ) );
     connect( Cancel, SIGNAL( clicked()), this, SLOT( slot_Cancel() ) );

this->show();
 this->setMinimumSize(this->minimumSizeHint()); //needed since QT4 to make sure the widget is not shown minimized
}
GDosoptions::~GDosoptions(){
}


void GDosoptions::slot_OK(){
    emit accept();
}
void GDosoptions::slot_Cancel(){
    //gather results when OK
    emit reject();
}

size_t GDosoptions::getdegree(){
    return size_t(getnumber(nmaxedit->text()));
}
bool GDosoptions::getsumrule(){
    return sumrulebutton->isChecked();
}
size_t GDosoptions::getinterpolation(){
    return interpolchooser->currentIndex()+1; //return from 1 to n
    //1=linear
    //2=cubic spline
    //3=upsample
}

size_t GDosoptions::getbroadeningtype(){
     return lifetimechooser->currentIndex(); //return from 0 to n
}

double GDosoptions::getnumber(const QString& valstring)const{
  bool ok;
  double result=valstring.toDouble(&ok);
  if (ok) return result;
  else return 0.0;
}

double GDosoptions::getintegrationwidth()const{
  return getnumber(integrationedit->text());
}

double GDosoptions::getthreshold()const{
    double value=getnumber(thresholdedit->text())/100.0;
    if (value<0){
        value=0.0;
    }
    if (value>1.0){
        value=1.0;
    }
  return value; //return not in percent but as fraction
}


void GDosoptions::slot_changeinterpol(int index) {
        //update the lifetime broadening state
        if ((index==1)||(index==0)){
            lifetimechooser->setEnabled(true);
        }
        else{
            lifetimechooser->setEnabled(false);
        }
    }
