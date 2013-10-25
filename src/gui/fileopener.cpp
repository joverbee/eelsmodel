/***************************************************************************
                          fileopener.cpp  -  description
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

#include "src/gui/fileopener.h"

//#define FILEOPEN_DEBUG

#include <iostream>
#include <string>

#include <QFileInfo>
#include <QString>

#include "src/core/eelsmodel.h"

#include "src/gui/getfilename.h"

Eelsmodel* geteelsmodelptr();
QWorkspace* getworkspaceptr();

Fileopener::Fileopener(QWidget *parent, const char *name ,std::string fname) : QWidget(parent)
{
dirname="";
filter="*.*";
name="open file dialog";
caption="Choose a file";
filename=QString::fromStdString(fname);

projfile=QString::fromStdString((geteelsmodelptr())->getprojectfilename());
QFileInfo fil=QFileInfo(projfile);
//take only the path
modelpath=fil.absolutePath(); //return absolute path of the file
#ifdef FILEOPEN_DEBUG
  std::cout << "fileopener\n";
  std::cout << "filename: " <<filename.toStdString()<<"\n";
  std::cout << "projfile: " <<projfile.toStdString()<<"\n";
  std::cout << "modelpath: " <<modelpath.toStdString()<<"\n";
#endif
}

Fileopener::~Fileopener(){
}

std::string Fileopener::open(){
//try finding the file with the given name
QFileInfo fi=QFileInfo(filename);
if (fi.exists()) return filename.toStdString();
#ifdef FILEOPEN_DEBUG
 std::cout << "the filename as is was not found\n";
#endif
//strip the path and try finding it with the modelpath
const QString strippedfilename= fi.fileName();
const QString filenamempath=(QString(modelpath).append("/") ).append(strippedfilename);
QFileInfo fimpath=QFileInfo(filenamempath);
#ifdef FILEOPEN_DEBUG
 std::cout << "trying a to take the path of the model and create a new filename\n";
 std::cout << "strippedfilename: " <<strippedfilename.toStdString()<<"\n";
 std::cout << "newfilename: " <<filenamempath.toStdString()<<"\n";
#endif
if (fimpath.exists()) return filenamempath.toStdString();
#ifdef FILEOPEN_DEBUG
 std::cout << "still no succes, trying a file dialog instead\n";
#endif
//if still not found open a file dialog and let the user find the file
Getfilename myfile(0,"Select a file to load");
myfile.setfilter(strippedfilename.toStdString());
myfile.setcaption("file not found, please point to the file or cancel");
filename=QString::fromStdString(myfile.open());
return filename.toStdString();//conversion of QString to const char*
}









