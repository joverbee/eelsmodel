/***************************************************************************
                          integerinput.cpp  -  description
Ask for an integer value
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

#include "src/gui/integerinput.h"

#include <QInputDialog>

Integerinput::Integerinput(QWidget *parent, const char *name,const char* message,int& value,const int min,const int max) : QWidget(parent) {
  int step=1;
  bool ok = FALSE;
  int res = QInputDialog::getInteger(this,name,message,value,min,max,step, &ok );


  if ( ok )
      value=res;// user entered something and pressed OK
  else
      value=0;// user pressed Cancel
}

Integerinput::~Integerinput(){
}
