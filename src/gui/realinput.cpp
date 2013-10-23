/***************************************************************************
                          realinput.cpp  -  description
                             -------------------
    begin                : Wed May 14 2003
    copyright            : (C) 2003 by Jo Verbeeck
    email                : joverbee@ruca.ua.ac.be
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#include "src/gui/realinput.h"
#include <qinputdialog.h>




RealInput::RealInput(QWidget *parent, const char *name,const char* message,double& value,const double min,const double max)
: QWidget(parent) {
  int decimals=5;
  bool ok = FALSE;
  //double res = QInputDialog::getDouble(name,message,value,min,max,decimals, &ok, this );
  double res = QInputDialog::getDouble(this,name,message,value,min,max,decimals, &ok );

  if ( ok )
      value=res;// user entered something and pressed OK
  else
      value=1.0;// user pressed Cancel
}

RealInput::~RealInput(){
}
