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
 * Copyright: 2003-2013 Jo Verbeeck
 *
 **/

/**
 * eelsmodel - core/multispectrum.h
 **/

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

  void display(QMdiArea *parent);
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
