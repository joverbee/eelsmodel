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
 * eelsmodel - gui/integerinput.cpp
 **/

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
