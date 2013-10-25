/***************************************************************************
                          multispectrum.h  -  description
                             -------------------
    begin                : Sat Feb 22 2003
    copyright            : (C) 2003 by Jo Verbeeck
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

#ifndef MULTISPECTRUM_H
#define MULTISPECTRUM_H

#include <vector>
#include <string>

#include "src/core/spectrum.h"
#include "src/gui/graph.h"

class Imagedisplay;

/* An array of spectra together, indexed by a number. Can be used everywhere where a normal spectrum can be used. You just get the current indexed spectrum */

class Multispectrum : public Spectrum  {
  std::vector<Spectrum*> dataset;
  std::string mspectrumname;
  std::string mspecfilename;
  unsigned int currspectrum;
  Graph* graphptr;
  Imagedisplay* imdisplayptr;
  QWidget *parentptr;
  bool b2D;
  size_t stride;
public:
  Multispectrum();
  ~Multispectrum();
  void addspectrum(Spectrum* spec);

  void display(QWorkspace *parent);
  void update();
  void updatereload();
  std::string getname()const;
  std::string getfilename()const;
  Spectrum* getspectrum(unsigned int index)const;
  unsigned int getnpoints()const;
  unsigned int getcurrentslice()const;
  Spectrum* getcurrentspectrum()const;
  unsigned int getsize()const;
  bool indexOK(unsigned int index)const;
  double getmin()const;
  double getmax()const;
  void setname(std::string namestr);
  void setfilename(std::string namestr);
  void setpppc(double pppc);
  double getpppc()const;
  void setcurrentslice(unsigned int index);
  void setcurrentslicenoupdate(unsigned int index);
  const Imagedisplay* getimageptr()const{return imdisplayptr;};
  void savedat(std::string filename); //save the whole multispectrum as a binary dat file
  bool is2D(){return b2D;}
  void set2D(bool b){b2D=b;}
  size_t getstride(){return stride;}
  void setstride(size_t s){stride=s;}

};

#endif
