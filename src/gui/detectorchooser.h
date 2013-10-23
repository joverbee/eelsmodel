/***************************************************************************
                          detectorchooser.h  -  description
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

#ifndef DETECTORCHOOSER_H
#define DETECTORCHOOSER_H

#include <qwidget.h>
/**A Qwidget to choose the type of detector to use
  *@author Jo Verbeeck
  */
class Model;
class DetectorChooser : public QWidget  {
   Q_OBJECT
public:
  DetectorChooser(QWidget *parent, const char *name,Model* mymodel);
  ~DetectorChooser();
  bool Readfile(QStringList& list, std::vector<double>& gainlist,  std::vector<double>& corrfactorlist);
};

#endif
