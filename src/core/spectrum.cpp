/***************************************************************************
                          spectrum.cpp  -  Class to contain EELS and EDS spectra
                             -------------------
    begin                : Sat Oct 12 2002
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

//#define DEBUGSPECTRUM
#include "src/core/spectrum.h"
#include "src/core/component.h"
#include <iostream>
#include <vector>
#include <iostream>
#include <qmatrix.h>
#include <string>
#include <cmath>
#include "src/gui/graph.h"
#include "src/gui/getfilename.h"
#include <fstream>
#include "src/gui/saysomething.h"
//#include <stdlib>
#include "src/gui/fileopener.h"



class QWorkspace;

Spectrum::~Spectrum(){
  //Problem: if we could to resource aqcuisisition is initialisation
  //cpp would clean up the graphptr by itself
  //important for safety: a destructor is not supposed to throw exceptions
  //can give problems with the STL library eg. when using spectrum in a vector or list (which is what we do all the time)


  //clean up the graph if you die
#ifdef DEBUGSPECTRUM
  std::cout << "deleting spectrum:"<< this->getname()<< "with graphptr:"<<graphptr <<"\n";
#endif

  try{
    if (graphptr!=0) {
      //PROBLEM deleted more than once...what is happening?
      //probably there are several copies of the same spectrum pointing to one graph
      //if these go out of scope they try to delete several times the same graph
      delete(graphptr);
      #ifdef DEBUGSPECTRUM
       std::cout << "deletinggraph at :"<< graphptr <<"\n";
      #endif
    }
  }
  catch(...){
    //didn`t work, this means the graph was not valid anyway
  }
  std::cout << "cleaned up a spectrum"<<std::endl;
}

//        constructors
Spectrum::Spectrum()
:dataset()
{
spectrumname="";
graphptr=0;    //strange graphptr is not initialised to 0 while it should be in ansi c++?
npoints=0;
pppc=1.0;
eshift=0.0;
setfilename("");
}


Spectrum::Spectrum(unsigned int n)
  :dataset(n) //get memory
{
              spectrumname="";
  graphptr=0;    //strange graphptr is not initialised to 0 while it should be in ansi c++?
  npoints=n;
  pppc=1.0;
  eshift=0.0;
  //create random data for testing purposes
  for (size_t i=0;i<n;i++){
    dataset[i].energy=double(i);
    setcounts(i,(double(rand())/RAND_MAX)*1000.0);
    seterror(i,1.0);
    setexclude(i,false);
    }
  setname("untitled");
  setxunits("energy");
  setyunits("counts");
  setfilename("");
}

Spectrum::Spectrum(unsigned int n,double estart, double dispersion)
  :dataset(n) //get memory

{
              spectrumname="";
graphptr=0;    //strange graphptr is not initialised to 0 while it should be in ansi c++?
  npoints=n;
  pppc=1.0;
  eshift=0.0;
  for (size_t bin=0;bin<npoints;bin++){
    dataset[bin].energy=estart+bin*dispersion;
    setcounts(bin,(double(rand())/RAND_MAX)*1000.0);
    setexclude(bin,false);
    }
  setname("untitled");
  setfilename("");
}

Spectrum::Spectrum(loadmsa l,std::string filename) //construct a spectrum loaded from disk in emsa format
:dataset()
//load an emsa-msa file from disk
//this format will become an ISO standard and currently has number ISO/DIS22029
//the current implementation is based on notes by RF Egerton and N Zalucec
{
      spectrumname="";
  graphptr=0;    //strange graphptr is not initialised to 0 while it should be in ansi c++?
  pppc=1.0;
  eshift=0.0;
  npoints=0;
  //setname("untitled");
  //get a filename from the Getfilename class



if (filename==""){
  Getfilename myfile(0,"Select a file");
  //myfile.setdirname("~");
  myfile.setfilter("Spectra (*.ems *.msa *.mas *.emsa *.EMS *.MAS *.EMSA *.MSA)");
  myfile.setcaption("Select a spectrum file");
  filename=myfile.open();
  if (filename=="") throw Spectrumerr::load_cancelled();//cancelled by user
  }

//better do it in the components because they have to store the new found filename
//test existence of filename and get a corrected filename if not found
Fileopener f(0,0,filename);
 filename=f.open();
//filename is now an updated name
if (filename=="") throw Spectrumerr::load_cancelled();//cancelled by user

  #ifdef DEBUGSPECTRUM
  std::cout <<"the filename is: "<<filename <<"\n";
  #endif
//open this file for reading ascii
std::ifstream msafile(filename.c_str());
if (!msafile) {
  throw Spectrum::Spectrumerr::load_error("Can not open file for reading");
  }

//show line per line what is encountered
std::string myline;
std::string::size_type mypos;

char eol='\n'; //eol, in future it should be determined from the file because it can be platform specific
char cr='\r';
double dE=0.0;
double Estart=0.0;

while (std::getline(msafile,myline,eol))
{
  if (myline.find(cr)!=std::string::npos) {
    myline.erase(myline.find(cr)); //remove carriage return from the end of the line
  }
  #ifdef DEBUGSPECTRUM
  std::cout <<myline <<"\n";
  #endif
  //convert first 13 characters of each line to uppercase
  //header keywords are allowed to be any mix of upper and lowercase
  for (unsigned int i=0;(i<13)&&(i<myline.length());i++){
    if (std::islower(myline[i])) myline[i]=std::toupper(myline[i]); //if so, replace by its uppercase equivalent
    }

  //look for specific header parts that are needed to construct the spectrum
  if (myline.find("#TITLE")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string title= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout <<"title " << title <<"\n";
      #endif
      this->setname(title);
      }
    }
  if (myline.find("#XUNITS")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string xun= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout <<"xunits " << xun <<"\n";
      #endif
      this->setxunits(xun);
      }
    }
    if (myline.find("#YUNITS")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string yun= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout <<"yunits " << yun <<"\n";
      #endif
      this->setyunits(yun);
      }
    }

  if (myline.find("#NPOINTS")!=std::string::npos){
    mypos=myline.find(": ");
    std::string npointstring;
    if (mypos!=std::string::npos){
      npointstring= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout << "npoints" << npointstring <<"\n";
      #endif
      int n=atoi(npointstring.c_str());
      #ifdef DEBUGSPECTRUM
      std::cout << "npoints converted to int " << n <<"\n";
      #endif
      //if (n>4096){
      //  throw Spectrum::Spectrumerr::load_error("Npoints is larger then 4096, which is forbidden in the EMSA standard V1.0");
      //  }
      this->setnpoints(n); //this results in loss of data and graph and resizes the dataset
      }
    }

    if (myline.find("#DATATYPE")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string dtype= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout << "datatype" << dtype <<"\n";
      #endif
      if (dtype=="XY") datatype=XY;
      if (dtype=="Y") datatype=Y;
      }
    }


   if (myline.find("#XPERCHAN")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string dispersion= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout << "dispersion" << dispersion <<"\n";
      #endif
      dE=atof(dispersion.c_str()); //convert to double
      #ifdef DEBUGSPECTRUM
      std::cout << "dispersion converted to double " << dE <<"\n";
      #endif
      }
    }
    if (myline.find("#OFFSET")!=std::string::npos){
    mypos=myline.find(": ");
    if (mypos!=std::string::npos){
      std::string offset= myline.substr(mypos+2);
      #ifdef DEBUGSPECTRUM
      std::cout << "estart" << offset <<"\n";
      #endif
      Estart=atof(offset.c_str()); //convert to double
      #ifdef DEBUGSPECTRUM
      std::cout << "estart converted to double " << Estart <<"\n";
      #endif
      }
    }
  if (myline.find("#SPECTRUM")!=std::string::npos) break;

}
//start to read the data
if (datatype==Y){
  //Y format
  std::string number;
  double energy,cts;
  int index=0;
  std::string::size_type previouspos=0;
  while (std::getline(msafile,myline,eol))
    {
    if (myline.find(cr)!=std::string::npos)  myline.erase(myline.find(cr)); //remove carriage return from the end of the line
    #ifdef DEBUGSPECTRUM
    std::cout <<myline <<"\n";
    #endif
    if (myline.find("#ENDOFDATA")!=std::string::npos) break;
    std::string piece;
    do{
      piece= myline.substr(previouspos,std::string::npos);
      //  std::cout <<"piece" <<piece<<"\n";
      mypos=piece.find(",");
      number=piece.substr(0,mypos);
      // cout <<number<<" : ";
      //convert number to double
      cts=atof(number.c_str());
      energy=Estart+double(index)*dE;
      // std::cout <<index<<" : "<<energy<<" : "<<cts<<"\n";
      this->setdatapoint(index++,energy,cts,0.0);
      previouspos+=mypos+1;
      }
    while (mypos!=std::string::npos);
    previouspos=0;
    #ifdef DEBUGSPECTRUM
    std::cout <<"\n";
    #endif
  }
}
else {
  //XY format
  std::string number;
  double energy,cts;
  int index=0;
  std::string::size_type previouspos=0;
  while (std::getline(msafile,myline,eol))
    {
    if (myline.find(cr)!=std::string::npos) {
      myline.erase(myline.find(cr)); //remove carriage return from the end of the line
    }
    #ifdef DEBUGSPECTRUM
    std::cout <<myline <<"\n";
    #endif
    if (myline.find("#ENDOFDATA")!=std::string::npos) break;
    std::string piece;
    do{
      piece= myline.substr(previouspos,std::string::npos);
      //  std::cout <<"piece" <<piece<<"\n";
      mypos=piece.find(",");
      number=piece.substr(0,mypos);

      //the first number is the energy
      energy=atof(number.c_str());

      //read the second number
      previouspos+=mypos+1;
      piece= myline.substr(previouspos,std::string::npos);
      mypos=piece.find(",");
      number=piece.substr(0,mypos);
      //convert number to double
      cts=atof(number.c_str());
      //energy=Estart+double(index)*dE;
      #ifdef DEBUGSPECTRUM
      std::cout <<index<<" : "<<energy<<" : "<<cts<<"\n";
      #endif
      this->setdatapoint(index++,energy,cts,0.0);
      previouspos+=mypos+1;
      }
    while (mypos!=std::string::npos);
    previouspos=0;
    #ifdef DEBUGSPECTRUM
    std::cout <<"\n";
    #endif
    }
  }
//there can still be a #CHECKSUM at the end of the file, but we ignore it here...
setfilename(filename);
this->initpoissonerror();

}

//    other member functions

unsigned int Spectrum::getnpoints()const{return npoints;}

void Spectrum::setnpoints(int n)
{
//all data will be erased!!!!
  try{
    dataset.resize(n);
  }
  catch(...){
    //allocation problem
    throw Spectrumerr::bad_alloc();
  }
npoints=n;
//delete any graphs if they would exist
 if (graphptr!=0) delete graphptr;
}
double Spectrum::getpppc()const{return pppc;}
double Spectrum::geteshift()const{return eshift;}

void Spectrum::setpppc(double p){
  pppc=p;
  this->initpoissonerror();
}

void Spectrum::seteshift(double e){
  eshift=e;
}

Spectrum& Spectrum::operator+=(const Spectrum& a1)
{
  if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
  //copy vectors
  for (unsigned int bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts+=(a1.dataset[bin]).counts;
  }
return *this;
}



Spectrum& Spectrum::operator*=(const Spectrum& a1)
{
  //IMPORTANT pass by byref otherwise an expensive copy is made and the destructor is called when a1 goes out of scope
  //this causes the graph to be deleted many times causing a pointer error
  if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
  //do the operator
  for (size_t bin=0;bin<npoints;bin++)
    {
    this->setcounts(bin,((this->getcounts(bin))*(a1.getcounts(bin))));
  }
return *this;
}

Spectrum& Spectrum::operator-=(const Spectrum& a1)
{
  if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
  //copy vectors
  for (size_t bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts-=(a1.dataset[bin]).counts;
  }
return *this;
}

Spectrum& Spectrum::operator+=(const double d)
{
  for (size_t bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts+=d;
  }
return *this;
}

Spectrum& Spectrum::operator-=(const double d)
{
  for (size_t bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts-=d;
  }
return *this;
}

Spectrum& Spectrum::operator/=(const Spectrum& a1)
{
  if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
  //copy vectors
  for (size_t bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts/=(a1.dataset[bin]).counts;
  }
return *this;
}




Spectrum& Spectrum::operator/=(const double x)
{
  for (size_t bin=0;bin<npoints;bin++) {
    ((dataset[bin]).counts)/=x;
  }
return *this;
}

Spectrum& Spectrum::operator*=(const double x)
{
  for (size_t bin=0;bin<npoints;bin++) {
    ((dataset[bin]).counts)*=x;
  }
return *this;
}
/*
Spectrum Spectrum::operator=(const Component a1){
 if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
  //copy a1 contents in this
  for (int bin=0;bin<npoints;bin++)
    {
    (dataset[bin]).counts=(a1.dataset[bin]).counts;
  }
return *this;
}
*/
Spectrum& Spectrum::operator=(const Spectrum& a1){
  //copy assignment
  //copy all data but leave graphptr=0
  //this is required otherwise the two final spectra have the same graph
  //this makes that the original sample loses its monopoly over the graph


 //make sure they have same size
 if (a1.npoints!=npoints) {throw Spectrumerr::not_same_size();}
 //check for assignment to itself
 if (this != &a1){
  //copy a1 contents in this
  for (size_t bin=0;bin<npoints;bin++){
    //carefull with eshift!
    this->setdatapoint(bin,a1.getrawenergy(bin),a1.getcounts(bin),a1.geterrorbar(bin));
  }
  //copy all datafields
  this->setpppc(a1.getpppc());
  this->seteshift(a1.geteshift());
  this->setname(a1.getname());
  this->setxunits(a1.getxunits());
  this->setyunits(a1.getyunits());
  graphptr=0;
  }
  return *this;
}

Spectrum Spectrum::operator*(double x){

  for (size_t bin=0;bin<npoints;bin++)
    {
    ((dataset[bin]).counts)*=x;
  }
return *this;
}





void Spectrum::clear()
{
  //set all points to zero
  for (size_t bin=0;bin<npoints;bin++) {
    (dataset[bin]).counts=0.0;
    }
}
void Spectrum::setdatapoint (int index,double en,double cts,double err)
{
badindex(index);
dataset[index].energy=en-eshift; //correct for eshift, the energy that we think we are, is not the real energy because we read en+eshift via getenergy(i)
setcounts(index,cts);
seterror(index,err);
}
void Spectrum::seterror (int index,double err)
{
badindex(index);
//only positive error bars allowed
 if (err>=0.0){
   dataset[index].errorbar=err;
 }
 else{
   err=0.0;
 }
}

void Spectrum::setexclude(int index,bool b){
badindex(index);
dataset[index].exclude=b;
}

void Spectrum::setexcluderegion(int startindex,int endindex){
 for (int index=startindex;index<=endindex;index++) {
    setexclude(index,true);
    }
}

void Spectrum::resetexcluderegion(int startindex,int endindex){
for (int index=startindex;index<=endindex;index++) {
    setexclude(index,false);
    }
}
void Spectrum::resetexcluderegion(){
for (unsigned int index=0;index<npoints;index++) {
    setexclude(index,false);
    }
}

void Spectrum::setcounts (int index,double cts)
{
badindex(index);
//only positive counts allowed, otherwise problems with eg poisson distribution calculations etc
// if (cts>0.0){
//   dataset[index].counts=cts;
// }
// else{
   dataset[index].counts=0.0;
   // }
 dataset[index].counts=cts;
}

void Spectrum::getdatapoint(int index,double* en,double* cts,double* err)const
{
badindex(index);
*en=getenergy(index);
*cts=getcounts(index);
*err=geterrorbar(index);
}
double Spectrum::getcounts(int index)const
{
badindex(index);
return (dataset[index].counts)*pppc;
}
double Spectrum::getenergy(int index)const
{
badindex(index);
return dataset[index].energy+eshift;
}

double Spectrum::getrawenergy(int index)const
{
badindex(index);
return dataset[index].energy;
}

double Spectrum::geterrorbar(int index)const
{
badindex(index);
return dataset[index].errorbar;
}
bool Spectrum::isexcluded(int index)const{
badindex(index);
return dataset[index].exclude;
}

double Spectrum::getdispersion()const
{return (this->getenergy(1)-this->getenergy(0));}

void Spectrum::badindex(int index)const
{
if ((index<0) | (index>=int(npoints))) throw Spectrumerr::bad_index();
}
void Spectrum::display(QWorkspace* parent)
{
  //if we already have a graph, just update it
  if (graphptr==0){
     const char* name=Spectrum::getname();
    graphptr =new Graph(parent,name,this);
  }
  //we should have a graph now but just to be sure
  if (graphptr!=0){
    graphptr->updategraph(0,this);
    graphptr->setcaption(getname());
    graphptr->setxlabel(getxunits());
    graphptr->setylabel(getyunits());
    graphptr->show();
 }
}
const char* Spectrum::getname()const
{
return (spectrumname.c_str());
}

const char* Spectrum::getxunits()const
{
return (xunits.c_str());
}
const char* Spectrum::getyunits()const
{
return (yunits.c_str());
}



void Spectrum::setname(std::string s)
{
spectrumname=s;
}
void Spectrum::setxunits(std::string s)
{
xunits=s;
}
void Spectrum::setyunits(std::string s)
{
yunits=s;
}
void Spectrum::normalize(double s)
{
    //normalise so that spectrum has a total of s counts
    const double eps=1e-10;
    if (s<eps){s=1.0;}
    const double sum=this->cumsum();
    for (size_t bin=0;bin<npoints;bin++){
        const double cts=this->getcounts(bin);
        this->setcounts(bin,cts*s/sum);
  }
}
double Spectrum::cumsum()const
{
//total sum of all counts
double sum=0.0;
  for (size_t bin=0;bin<npoints;bin++){
    sum+=this->getcounts(bin);
  }
return sum;
}
unsigned int Spectrum::getmaxindex()const{
//get index of the maximum value point in the spectrum
int index=0;
double max=this->getcounts(0);
  for (size_t bin=0;bin<npoints;bin++){
    const double cts=this->getcounts(bin);
    if(cts>max){
      index=bin;
      max=cts;
      }
  }
return index;
}

size_t Spectrum::getfirsthigherthen(double x)const{
//get index of the first pixel with a value higher then x
size_t index=0;
  for (size_t bin=0;bin<npoints;bin++){
    const double cts=this->getcounts(bin);
    if(cts>x){
      index=bin;
      break;
    }
  }
return index;
}

double Spectrum::getmax()const{
double max=this->getcounts(0);
  for (size_t bin=0;bin<npoints;bin++){
    const double cts=this->getcounts(bin);
    if(cts>max) max=cts;
    }
return max;
}

double Spectrum::getmin()const{
double min=this->getcounts(0);
  for (size_t bin=0;bin<npoints;bin++){
    const double cts=this->getcounts(bin);
    if(cts<min) min=cts;
    }
return min;
}
void Spectrum::initpoissonerror(){
  //init the error bars to be sqrt(pppc*cts)
  //a poisson like sigma taking into account the primary particles per count
   for (size_t bin=0;bin<npoints;bin++){
    const double value=this->getcounts(bin)*this->getpppc();
    double error=0.0;
    if (value>0){//check validity before taking sqrt
        error=sqrt(value);
    }
    this->seterror(bin,error);
   }
}
void Spectrum::savedat(std::string filename){
     //save the whole spectrum as a binary dat file
     //open file for writing ascii
     try{
     std::ofstream fp(filename.c_str(), std::ios_base::binary); //open for writing, overwrite if already exists
     //do the writing
     for (unsigned int i=0;i<this->getnpoints();i++){
         const double val=this->getcounts(i);
         fp.write((char *)&val, sizeof(val));
     }
     //closing file not needed because handled in destructor of fp
     }
     catch(...){
     //not written
     }
}

void Spectrum::setenergy(int index,double e)
{
  badindex(index);
  dataset[index].energy=e;
}
size_t Spectrum::getenergyindex(double energy)const{
    //get the index of the point closes to energy
    //return 0 if energy is lower than lowest index
    //returns npoint if the energy is higher than the highest energy in this spectrum
      for (size_t i=0;i<this->getnpoints();i++){
          if (this->getenergy(i)>=energy) return i;
      }
      return this->getnpoints();
    }

void Spectrum::smoothgaussian(double sigma){
     //smooth data by convolution with a gaussian with stdev sigma

     //prepare Gaussian Kernel up till 3sigma on both sides
     size_t kernelsize=size_t((6.0*sigma)/this->getdispersion());
     std::vector<double> kernel;
     kernel.resize(kernelsize);
     for (size_t i=0;i<kernelsize;i++){
            kernel[i]=exp(-pow(double(i)-double(kernelsize)/2.0,2.0)/(2.0*pow(sigma/this->getdispersion(),2.0)));
     }
     //normalise the Kernel
     double totalint=0.0;
     for (size_t i=0;i<kernelsize;i++){
         totalint+=kernel[i];
     }

     //apply this Kernel to the spectral data and apply normalisation
     for (size_t j=0;j<npoints;j++){
        double value=0.0;
        for (size_t i=0;i<kernelsize;i++){
            if ((i+j)<npoints){ //no circular convolution
                value+=kernel[i]/totalint*this->getcounts(j+i);
            }
        }
        this->setcounts(j,value);
     }
}
void Spectrum::resize(size_t n){
	//resize the Spectrum
	dataset.resize(n); //get memory	
	graphptr=0;    
	npoints=n;	  	
}