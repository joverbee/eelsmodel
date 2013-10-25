/***************************************************************************
                          getfilename.cpp  -  description
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

#include "src/gui/getfilename.h"

//#define GETFILENAME_DEBUG

#include <string>
#include <iostream>

#include <QDir>
#include <QFileDialog>
#include <QFileInfo>
#include <QString>

QString globaldirname; //global variable holding the current directory, declared in getfilename.h

Getfilename::Getfilename(QWidget *parent, std::string nm ) : QWidget(parent)
{
    this->setWindowTitle(nm.c_str());
#ifdef GETFILENAME_DEBUG
  std::cout <<"entering getfilename \n";
#endif
name="";
filter="*.*";
name="open file dialog";
caption="Choose a file";
}


Getfilename::~Getfilename(){
}

std::string Getfilename::open(){
#ifdef GETFILENAME_DEBUG
  std::cout <<"entering getfilename::open() \n";
  std::cout <<"dirname set to:"<<globaldirname.toStdString()<<"\n";
#endif
  QString s="";
   try{
    s=QFileDialog::getOpenFileName(0,caption,globaldirname,filter);
    //strange in previous version of Qt the parent should be set to this
    //now only setting to 0 works
  }
  catch(...){
    std::cout << "cought an exception in getfilename::open \n";
  }
  //problem directory is not properly subtracted
  QFileInfo fil=QFileInfo(s);
  //take only the path
  globaldirname=fil.absolutePath (); //return absolute path of the file



#ifdef GETFILENAME_DEBUG
  std::cout <<"leaving getfilename::open() \n";
  std::cout <<"dirname set to:"<<globaldirname.toStdString()<<"\n";
 std::cout <<"filename set to:"<<s.toStdString()<<"\n";
#endif
  return s.toStdString();
}

std::string Getfilename::save(){
  QString s= QFileDialog::getSaveFileName(0,caption,globaldirname,filter);    //problem directory is not properly subtracted
  //strange before we needed paerent=this, now only parent=0 works
  QFileInfo fil=QFileInfo(s);
  //take only the path
  globaldirname=fil.absolutePath (); //return absolute path of the file

  return s.toStdString();
}

void Getfilename::setdirname(std::string dname){
  globaldirname=QString::fromStdString(dname);
#ifdef GETFILENAME_DEBUG
  std::cout <<"getfilename::setdirname() to:"<<globaldirname.toStdString()<<" \n";
#endif
}
void Getfilename::setfilter(std::string fstring){
  filter=QString::fromStdString(fstring);
}
void Getfilename::setname(std::string nm){
  name=QString::fromStdString(nm);
}
void Getfilename::setcaption(std::string capt){
  caption=QString::fromStdString(capt);
}







