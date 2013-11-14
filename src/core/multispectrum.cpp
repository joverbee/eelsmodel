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
 * eelsmodel - core/multispectrum.cpp
 **/

#include "src/core/multispectrum.h"

#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "src/core/spectrum.h"

#include "src/gui/graph.h"
#include "src/gui/imagedisplay.h"

Multispectrum::Multispectrum()
:dataset()
{
currspectrum=0;
graphptr=0;
imdisplayptr=0;
stride=1;
b2D=false;
}


Multispectrum::~Multispectrum(){
//clean up all spectra, because we own them from the moment they are added to a multispectrum
/* for (size_t i=0;i<dataset.size();i++){
    if (dataset[i]!=0) delete(dataset[i]);
 }
 //and clean up the graph as well, but first check if it is really still there
 try{
    int j=graphptr->getnplots();
    j=0; //supress unused warning
 }
 catch(...)
 {
     graphptr=0;
 }
 if (graphptr!=0) delete(graphptr);
 std::cout << "cleaned up the multispectrum"<<std::endl;
 */
}

void Multispectrum::addspectrum(Spectrum* spec){
    //from now on, we own them and we should also delete them
  if (dataset.size()!=0){
    //check if new spec has same size as previous
    if (dataset[0]->getnpoints()!=spec->getnpoints()) return;
    }

  //make sure filename of each single spectrum is filename of multispectrum
  spec->setfilename(this->getfilename());
  dataset.push_back(spec);
  currspectrum=dataset.size()-1;
}

void Multispectrum::display(QWorkspace *parent){
/*  //show current spectrum
  Spectrum* specptr=this->getcurrentspectrum();
  if (specptr==0) return; //no valid spectrum yet
  if (graphptr==0) {
    graphptr=new Graph(parent,getname().c_str(),this);
  }
  else  graphptr->reinit(); //if the graph already exists, only redraw it
  graphptr->show();

  //draw or update the image view and show which slice we are working with
  if (imdisplayptr==0) imdisplayptr=new Imagedisplay(getname().c_str(),this);
  else  imdisplayptr->update();*/
}

void Multispectrum::update(){
//update the graph when somebody changes the current slice
if (graphptr!=0) graphptr->reinit();
//draw or update the image view and show which slice we are working with
if (imdisplayptr!=0) imdisplayptr->update();

}

void Multispectrum::updatereload(){
//update the graph when somebody changes the current slice
//but also reload the mspec and redisplay it...only needed when the mspec has changed
//takes more time than update
if (graphptr!=0) graphptr->reinit();
//draw or update the image view and show which slice we are working with
if (imdisplayptr!=0) imdisplayptr->updatereloadmspec();
}





std::string Multispectrum::getname()const{
  return mspectrumname;
}
std::string Multispectrum::getfilename()const{
  return mspecfilename;
}
Spectrum* Multispectrum::getspectrum(unsigned int index)const{
if (indexOK(index)) return dataset[index];
else return 0;
}

unsigned int Multispectrum::getnpoints()const{
return (this->getspectrum(0))->getnpoints();
}

unsigned int Multispectrum::getcurrentslice()const{
return currspectrum;
}

Spectrum* Multispectrum::getcurrentspectrum()const{
return getspectrum(currspectrum);
}

void Multispectrum::setcurrentslice(unsigned int index){
if (indexOK(index)) currspectrum=index;
update();
}

void Multispectrum::setcurrentslicenoupdate(unsigned int index){
if (indexOK(index)) currspectrum=index;
//update(); //no update
}

unsigned int Multispectrum::getsize()const{
return dataset.size();
}

double Multispectrum::getpppc()const{
return getspectrum(currspectrum)->getpppc();
}

double Multispectrum::getmin()const{
double min=getspectrum(0)->getmin();
for (unsigned int i=1;i<getsize();i++){
  if (getspectrum(i)->getmin()<min) min=getspectrum(i)->getmin();
  }
return min;
}

double Multispectrum::getmax()const{
double max=getspectrum(0)->getmax();
for (unsigned int i=1;i<getsize();i++){
  if (getspectrum(i)->getmax()>max) max=getspectrum(i)->getmax();
  }
return max;
}

void Multispectrum::setname(std::string namestr){
  mspectrumname=namestr;
}

void Multispectrum::setfilename(std::string namestr){
  mspecfilename=namestr;
}

bool Multispectrum::indexOK(unsigned int index)const{
return (index<dataset.size());
}
void Multispectrum::setpppc(double pppc){
  //set the pppc in all spectra of the multispectrum
  for (unsigned int i=0;i<getsize();i++){
    getspectrum(i)->setpppc(pppc);
  }
}
void Multispectrum::savedat(std::string filename){
     //save the whole multispectrum as a binary dat file
     //read it a real*8 in DM
     try{
     std::ofstream fp(filename.c_str(), std::ios_base::binary); //open for writing, overwrite if already exists
     //do the writing
     for (unsigned j=0;j<this->getsize();j++){
         Spectrum * myspec=this->getspectrum(j);
          for (unsigned int i=0;i<myspec->getnpoints();i++){
             const double val=myspec->getcounts(i);
             fp.write((char *)&val, sizeof(val));
         }
     }
     //closing file not needed because handled in destructor of fp
     }
     catch(...){
     //not written
     }


}
