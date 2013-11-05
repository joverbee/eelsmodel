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
 * eelsmodel - gui/realinput.cpp
 **/

#include "src/gui/realinput.h"

#include <QInputDialog>

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
