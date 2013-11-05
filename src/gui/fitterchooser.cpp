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
 * eelsmodel - gui/fitterchooser.cpp
 **/

#include "src/gui/fitterchooser.h"

#include <QInputDialog>
#include <QString>
#include <QStringList>

#include "src/fitters/fitter.h"

FitterChooser::FitterChooser(QWidget *parent, const char *name,int& retvalue ) : QWidget(parent) {
QStringList list;
QString s1="Least Squares Fitter";
list.append( s1 );
QString s2= "Weighted Least Squares Fitter (ML for normal distribution)";
list.append( s2 );
QString s3= "Maximum Likelyhood Fitter for Poisson Statistics";
list.append( s3 );

QString s4= "GSL: Least Squares";
list.append( s4 );

QString s5= "Levenberg Marquardt Poisson";
list.append( s5 );

bool ok=false;
QString result=QInputDialog::getItem (this,"Fitter Chooser","Choose a fitter type to use",list,0,false,&ok,0);

  if ( ok ){
    if (result==s1){
      retvalue=0;
      }
    if (result==s2){
      retvalue=1;
      }
    if (result==s3){
      retvalue=2;
      }
//disable GSL fitter on non unix system
     if (result==s4){
      retvalue=3;
      }
     if (result==s5){
      retvalue=4;
    }

    if ((result!=s5)&&(result!=s4)&&(result!=s3)&&(result!=s2)&&(result!=s1)){
      retvalue=-1;
      }
}
  else
      retvalue=-1;// user pressed Cancel
  
  std::cout << "retvalue was"<<retvalue<<"\n";
}
FitterChooser::~FitterChooser(){
}
