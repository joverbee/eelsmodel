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
 * eelsmodel - core/loadfiles.cpp
 **/

#include "src/core/loadfiles.h"

//#define DEBUGLOADFILES

#include <fstream>
  using std::ifstream;
#include <memory>
  using std::unique_ptr;
#include <string>
  using std::string;
#include <vector>
  using std::vector;

#include "src/core/multibyte.h"
#include "src/core/multispectrum.h"
#include "src/core/spectrum.h"
#include "src/core/tag.h"

//TODO
/*
unique_ptr<Spectrum> loadSpectrum(const string& filename)
{
  enum datatype { Y, XY} datatype;
  string name, xunits, yunits;
  double Estart, dE;

  unique_ptr<Spectrum> spectrum(new Spectrum());
  ifstream msafile(filename.c_str());
  if(!msafile)
    throw Spectrum::Spectrumerr::load_error("Can not open file for reading");

  //show line per line what is encountered
  string line;

  double dE=0.0;
  double Estart=0.0;

  while(std::getline(msafile,line))
  {
    //remove carriage return from the end of the line
    if(line.back() == '\r')
      line.pop_back();
#ifdef DEBUGLOADFILES
    std::cout <<line <<"\n";
#endif
    //convert first 13 characters of each line to uppercase
    //header keywords are allowed to be any mix of upper and lowercase
    for(std::size_t i=0; (i<13) && (i<line.length()); ++i)
      line[i]=std::toupper(line[i]);

    //look for specific header parts that are needed to construct the spectrum
    if(line.find("#TITLE") != string::npos)
    {
      string::size_type mypos = line.find(": ");
      if(mypos != string::npos)
      {
        spectrum->name = line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout <<"title " << spectrum->name <<"\n";
#endif
      }
    }
    if(line.find("#XUNITS") != string::npos)
    {
      string::size_type mypos = line.find(": ");
      if(mypos != string::npos)
      {
        spectrum->xunits = line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout <<"xunits " << spectrum->xunits <<"\n";
#endif
      }
    }
    if(line.find("#YUNITS") != string::npos)
    {
      string::size_type mypos=line.find(": ");
      if(mypos != string::npos)
      {
        spectrum->yunits = line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout <<"yunits " << spectrum->yunits <<"\n";
#endif
      }
    }

    if(line.find("#NPOINTS") != string::npos)
    {
      mypos = line.find(": ");
      string npointstring;
      if(mypos != string::npos)
      {
        //TODO: use std::stoi
        npointstring = line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout << "npoints" << npointstring <<"\n";
#endif
        int n=atoi(npointstring.c_str());
#ifdef DEBUGLOADFILES
        std::cout << "npoints converted to int " << n <<"\n";
#endif
        //if (n>4096){
        //  throw Spectrum::Spectrumerr::load_error("Npoints is larger then 4096, which is forbidden in the EMSA standard V1.0");
        //  }
        spectrum->datasetnpoints(n); //this results in loss of data and graph and resizes the dataset
      }
    }

    if (line.find("#DATATYPE")!=std::string::npos){
      mypos=line.find(": ");
      if (mypos!=std::string::npos){
        std::string dtype= line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout << "datatype" << dtype <<"\n";
#endif
        if (dtype=="XY") datatype=XY;
        if (dtype=="Y") datatype=Y;
      }
    }


    if (line.find("#XPERCHAN")!=std::string::npos){
      mypos=line.find(": ");
      if (mypos!=std::string::npos){
        std::string dispersion= line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout << "dispersion" << dispersion <<"\n";
#endif
        dE=atof(dispersion.c_str()); //convert to double
#ifdef DEBUGLOADFILES
        std::cout << "dispersion converted to double " << dE <<"\n";
#endif
      }
    }
    if (line.find("#OFFSET")!=std::string::npos){
      mypos=line.find(": ");
      if (mypos!=std::string::npos){
        std::string offset= line.substr(mypos+2);
#ifdef DEBUGLOADFILES
        std::cout << "estart" << offset <<"\n";
#endif
        Estart=atof(offset.c_str()); //convert to double
#ifdef DEBUGLOADFILES
        std::cout << "estart converted to double " << Estart <<"\n";
#endif
      }
    }
    if (line.find("#SPECTRUM")!=std::string::npos) break;

  }
  //start to read the data
  if (datatype==Y){
    //Y format
    std::string number;
    double energy,cts;
    int index=0;
    std::string::size_type previouspos=0;
    while (std::getline(msafile,line))
    {
      //remove carriage return from the end of the line
      if(line.back() == '\r')
        line.pop_back();
#ifdef DEBUGLOADFILES
      std::cout <<myline <<"\n";
#endif
      if (line.find("#ENDOFDATA")!=string::npos) break;
      string piece;
      do{
        piece= line.substr(previouspos,string::npos);
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
      while (mypos!=string::npos);
      previouspos=0;
#ifdef DEBUGLOADFILES
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
    while (std::getline(msafile,line))
    {
      //remove carriage return from the end of the line
      if(line.back() == '\r')
        line.pop_back();
#ifdef DEBUGLOADFILES
      std::cout <<myline <<"\n";
#endif
      if (line.find("#ENDOFDATA")!=std::string::npos) break;
      std::string piece;
      do{
        piece= line.substr(previouspos,std::string::npos);
        //  std::cout <<"piece" <<piece<<"\n";
        mypos=piece.find(",");
        number=piece.substr(0,mypos);

        //the first number is the energy
        energy=atof(number.c_str());

        //read the second number
        previouspos+=mypos+1;
        piece= line.substr(previouspos,std::string::npos);
        mypos=piece.find(",");
        number=piece.substr(0,mypos);
        //convert number to double
        cts=atof(number.c_str());
        //energy=Estart+double(index)*dE;
#ifdef DEBUGLOADFILES
        std::cout <<index<<" : "<<energy<<" : "<<cts<<"\n";
#endif
        this->setdatapoint(index++,energy,cts,0.0);
        previouspos+=mypos+1;
      }
      while (mypos!=std::string::npos);
      previouspos=0;
#ifdef DEBUGLOADFILES
      std::cout <<"\n";
#endif
    }
  }
  //there can still be a #CHECKSUM at the end of the file, but we ignore it here...
  setfilename(filename);
  this->initpoissonerror();

}
}
*/



