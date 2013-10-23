/***************************************************************************
                          detectorchooser.cpp  -  description
This widget lets the user choose between different detectors that are stored in the file detectors.txt
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

#include "src/gui/detectorchooser.h"

#include <qinputdialog.h>
#include <qstring.h>
#include <qstringlist.h>
#include "src/core/model.h"
#include "saysomething.h"
#include <fstream>
#include <qapplication>



//#define COMPONENT_DEBUG

DetectorChooser::DetectorChooser(QWidget *parent, const char *name,Model* mymodel) : QWidget(parent) {
  QStringList list;
  std::vector<double> gainlist;
  std::vector<double> corrfactorlist;
  this->Readfile(list,gainlist,corrfactorlist);
  bool ok=false;
  QString result=QInputDialog::getItem (this,"Detector Chooser","Choose a detector type to use",list,0,false,&ok);


  if ( ok ){
    //find out which number in the list
    //go trough the name list
    QStringList::Iterator it;
    int i=0;
    for (it = list.begin(); it != list.end(); ++it ) {
    if (result==*it){
	//found! update the gainfactor and the correlation factor
	std::string detname=(*it).toStdString () ;
#ifdef COMPONENT_DEBUG
       std::cout <<"selected the detector with name:"<<detname<<" occurence number i="<<i<<" gain="<<gainlist[i]<<" corrfactor:"<<corrfactorlist[i]<<"\n";
#endif
    if (mymodel==0){
       Saysomething mysay(0,"Error","No valid model present, can not set detector without model");
       return;
    }
	mymodel->setdetectorname(detname);
	mymodel->setdetectorgain(gainlist[i]);
	mymodel->setdetectorcorrfactor(corrfactorlist[i]);
	return;
      }
      i++;
    }
    if (it==list.end()){
      //nothing found
      Saysomething mysay(0,"Error","Something went wrong in selecting detector...you can only select existing entries in the detectors.txt file");
      return;
    }

}
  else
  // user pressed Cancel
  return;
}
DetectorChooser::~DetectorChooser(){
}

bool DetectorChooser::Readfile(QStringList& list, std::vector<double>& gainlist,  std::vector<double>& corrfactorlist){
//open file for reading
  std::ifstream detfile("detectors.txt",std::ios_base::binary);

  if (!detfile) {
    //complain that file can not be opened
    Saysomething mysay(0,"Error","The file detectors.txt could not be opened \n make sure it is in the executable directory");
    return false;
  }
  std::string detname;
  std::string gainstring;
  std::string corrfactorstring;
  //read line by line
  const char eol=(char)10;

  const char tab='\t';
  while (std::getline(detfile,detname,tab)){
#ifdef COMPONENT_DEBUG
    std::cout <<"detname:"<<detname<<"\n";
#endif
    if (detname.find("%")==std::string::npos){
      //a line without % sign found
      //split myline in tree parts according to name gain and corrfactor

      if (!std::getline(detfile,gainstring,tab)){
      	//error reading gain
       	Saysomething mysay(0,"Error","Error reading gain factor");
        break;
      }
      if(!std::getline(detfile,corrfactorstring,eol)){
      Saysomething mysay(0,"Error","Error reading correlation factor");
	  //error reading corrfactor
      break;
      }
      //add these values to the list
#ifdef COMPONENT_DEBUG
       std::cout <<"detname:"<<detname<<"gainstring"<<gainstring<<"corrfactorstring"<<corrfactorstring<<"\n";
#endif
      char* end;
      const double corrfactor=std::strtod( corrfactorstring.c_str(), &end);
      const double gainfactor=std::strtod( gainstring.c_str(), &end);
      list.append(QString::fromStdString (detname));
      gainlist.push_back(gainfactor);
      corrfactorlist.push_back(corrfactor);
#ifdef COMPONENT_DEBUG
       std::cout <<"detname:"<<detname<<"gain (double)"<<gainfactor<<"corrfactor (double)"<<corrfactor<<"\n";
#endif

    }
    else{
         //a % line was read skip rest of line
         std::getline(detfile,corrfactorstring,eol);
         }

  }
  detfile.close();
  return true; //succes
}
