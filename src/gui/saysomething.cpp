/***************************************************************************
                          saysomething.cpp  -  description
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

#include "src/gui/saysomething.h"
#include <qmessagebox.h>

Saysomething::Saysomething(QWidget *parent, const char *name,const char* message,bool error ) : QWidget(parent) {
      QMessageBox msgbox;
      if (error){
        msgbox.critical( this,name,message);
      }
      else
      {
        msgbox.information( this,name,message);
      }
      msgbox.show();
}

Saysomething::~Saysomething(){
}
