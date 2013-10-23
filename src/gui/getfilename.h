/***************************************************************************
                          getfilename.h  -  description
                             -------------------
    begin                : Fri Nov 1 2002
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

#ifndef GETFILENAME_H
#define GETFILENAME_H

#include <qwidget.h>
#include <string>
#include <qstring.h>

/**Qt implementation to select a file from a list
  *@author Jo Verbeeck
  */
class Getfilename : public QWidget  {
Q_OBJECT
private:

public: 
QString filter;
QString name;
QString caption;
Getfilename(QWidget *parent=0, std::string nm=0);
~Getfilename();
std::string open();
std::string save();
void setdirname(std::string dname);//set the working directory
void setfilter(std::string fstring);
void setname(std::string nm);
void setcaption(std::string capt);
};

#endif
