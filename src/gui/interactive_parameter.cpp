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
 * eelsmodel - gui/interactive_parameter.cpp
 **/

#include "src/gui/interactive_parameter.h"

//#define DEBUG

#include <iostream>

#include <QCheckBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayout>
#include <QLineEdit>
#include <QPalette>
#include <QPushButton>
#include <QRadioButton>
#include <QValidator>
#include <QVBoxLayout>

#include "src/core/parameter.h"

Interactive_Parameter::Interactive_Parameter(QWidget *parent, const char *name,const Parameter* initialparameter,const std::string& description)
: QDialog(parent)
{
    //QDialog(parent,name,true,0)
    //fill out initial values of the parameter
    value=initialparameter->getvalue();
    std::string parname=initialparameter->getname();
    upperbound=initialparameter->getupperbound();
    lowerbound=initialparameter->getlowerbound();
    bound=initialparameter->isbound();
    canchange=initialparameter->ischangeable();
    displayed=initialparameter->isdisplayed();
    
    //
    int fieldwidth=-1;
    char format='e';
    int precision=6;
    //create a fill out box with parameter values
    this->setWindowTitle("Parameter Edit");
    parameter_name=parname.c_str();

    // Create Widgets which allow easy layouting
    QGridLayout* lay = new QGridLayout( this );
    lay->setMargin(6);
    lay->setSpacing(2);

    //validator check for numeric input and check bounds
    double_validator=new QDoubleValidator(this);
    bound_validator=new QDoubleValidator(this);
    
    //important! solves issues with . and , 
    double_validator->setLocale(QLocale::C);
    bound_validator->setLocale(QLocale::C);
       

    //label with explanation
    //mylabel=new   QLabel(description.c_str(),this);
    //lay->addWidget(mylabel,0,0,1,2);

    //parameter name
    parnameedit=new QLineEdit(parameter_name,this);
    parnamelabel=new QLabel("Parameter name",this);
    lay->addWidget(parnameedit,0,0);
    lay->addWidget(parnamelabel,0,1);

    (void) parnamelabel; //get rid of unused variable warning
    //parameter value
    QString valstring;
    valstring="%1"; //this is required on MacosX, but not on linux? maybe a version problem in Qt
    parvaledit=new QLineEdit(valstring.arg (value,fieldwidth,format,precision ),this);
    parvallabel=new QLabel("Parameter value",this);
    lay->addWidget(parvaledit,1,0);
    lay->addWidget(parvallabel,1,1);
    parvaledit->setValidator (double_validator);  //make sure only doubles are entered
    parvaledit->setEnabled(canchange);

    // insert 2 checkboxes
    check_change=new QCheckBox( "&Changeable", this);
    check_bound = new QCheckBox( "Bound", this);
    check_change->setChecked( canchange );
    check_bound->setChecked( bound );
    lay->addWidget(check_change,2,0);
    lay->addWidget(check_bound,2,1);

    //boundary values
    lowerboundedit=new QLineEdit(valstring.arg (lowerbound,fieldwidth,format,precision),this);
    lowboundlabel=new QLabel("Lower boundary",this);
    lay->addWidget(lowerboundedit,3,0);
    lay->addWidget(lowboundlabel,3,1);

    upperboundedit=new QLineEdit(valstring.arg (upperbound,fieldwidth,format,precision),this);
    upboundlabel=new QLabel("Upper boundary",this);
    lay->addWidget(upperboundedit,4,0);
    lay->addWidget(upboundlabel,4,1);

    lowerboundedit->setEnabled(bound);
    upperboundedit->setEnabled(bound);
    lowerboundedit->setValidator (double_validator);
    upperboundedit->setValidator (double_validator);

    check_displayed=new QCheckBox( "Display param. in graph", this);
    check_displayed->setEnabled(true);
    check_displayed->setChecked( displayed );        
    lay->addWidget(check_displayed,5,0);
                
    //OK button
     OK = new QPushButton( "&OK", this );
     lay->addWidget(OK,6,0,1,2);


    //connections
     connect( check_bound, SIGNAL( toggled(bool)), lowerboundedit, SLOT( setEnabled(bool) ) );
     connect( check_bound, SIGNAL( toggled(bool)), upperboundedit, SLOT( setEnabled(bool) ) );
     connect( lowerboundedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_lower_bound(const QString&) ) );
     connect( upperboundedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_upper_bound(const QString&) ) );
     connect( parvaledit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_val(const QString&) ) );
     connect( parnameedit, SIGNAL( textChanged(const QString&)), this, SLOT( slot_change_name(const QString&) ) );
     connect( check_bound, SIGNAL( toggled(bool)), this, SLOT( slot_change_bound(bool) ) );
     connect( check_change, SIGNAL( toggled(bool)), this, SLOT( slot_change_canchange(bool) ) );
     connect( check_displayed, SIGNAL( toggled(bool)), this, SLOT( slot_change_displayed(bool) ) );
     connect( OK, SIGNAL( clicked()), this, SLOT( slot_ok() ) );
}
Interactive_Parameter::~Interactive_Parameter(){
    
}
void Interactive_Parameter::slot_change_upper_bound(const QString& upstring){
  upperbound=getnumber(upstring);
  //change the validator for parvaledit to take the bound into account
  bound_validator->setTop(upperbound);
  parvaledit->setValidator(bound_validator);
#ifdef DEBUG
  std::cout <<"the upperbound is set to: "<<upperbound<<"\n";
#endif
  this->validate();
}

void Interactive_Parameter::slot_change_lower_bound(const QString& lowstring){
  lowerbound=getnumber(lowstring);
  //change the validator for parvaledit to take the bound into account
  bound_validator->setBottom(lowerbound);
  parvaledit->setValidator(bound_validator);
#ifdef DEBUG
  std::cout <<"the lowerbound is set to: "<<lowerbound<<"\n";
#endif
  this->validate();
}

void Interactive_Parameter::slot_change_val(const QString& valuestring){
  value=getnumber(valuestring);
#ifdef DEBUG
  std::cout <<"the value is set to: "<<value<<"\n";
#endif
  this->validate();
}

void Interactive_Parameter::slot_change_name(const QString& namestring){
  parameter_name=namestring;
}

double Interactive_Parameter::getnumber(const QString& valstring)const{
  bool ok;
  double result=valstring.toDouble(&ok);
  if (ok) return result;
  else return 0.0;
}

void Interactive_Parameter::slot_change_bound(bool boundstate){
  if (boundstate) {
    bound_validator->setTop(upperbound);
    bound_validator->setBottom(lowerbound);
    parvaledit->setValidator(bound_validator);
    }
  else  parvaledit->setValidator(double_validator);
  bound=boundstate;
  this->validate();
}
void Interactive_Parameter::slot_change_canchange(bool state){
  canchange=state;
  parvaledit->setEnabled(canchange);
}

void Interactive_Parameter::slot_change_displayed(bool state){
	displayed=state;
}


bool  Interactive_Parameter::validate(){
  QString mystring=parvaledit->text();
  int pos; //dummy to put cursor position, not used but needed for the function call validate(Qstring&,int&)     
  bool correct= ( bound_validator->validate(mystring,pos)==QDoubleValidator::Acceptable); 
#ifdef DEBUG
  std::cout <<"the value "<<mystring.toStdString() <<" is valid: "<<correct<<" and validator results="<<bound_validator->validate(mystring,pos)<<"\n";
  std::cout <<"the bounds are "<<(lowerboundedit->text()).toStdString()<<" "<<	(upperboundedit->text()).toStdString()<<"\n";	  		      
#endif
  QPalette qPalette;


  if (!correct) {
        qPalette.setColor(QPalette::Text, Qt::red);
      parvaledit->setPalette( qPalette ); //change textcolor to red
      parvaledit->update();
      }
  else  {
      qPalette.setColor(QPalette::Text, Qt::black);
      parvaledit->setPalette( qPalette ); //change textcolor back to normal
      parvaledit->update();

      }
return correct;
}

void Interactive_Parameter::slot_ok(){
  if (this->validate()) {
    emit(accept());
    }
  else {
    bound=false; //switch of boundaries if not met
    emit(accept());
    }
}
