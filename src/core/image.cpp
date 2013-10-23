/***************************************************************************
                          image.cpp  -  description
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
#include "src/core/image.h"
#include "src/core/multibyte.h"
#include "src/gui/saysomething.h"
#include "src/gui/getfilename.h"

#include "src/core/tag.h"
#include "src/core/spectrum.h"
#include "src/core/multispectrum.h"
#include <qwidget.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qapplication.h>
#include <qpushbutton.h>
#include <qrect.h>
#include <qcolor.h>
#include <qcursor.h>
#include <QPaintEvent>
#include <cmath>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
//covnert 2D SI to 1D SI?
//#define CONVERT2D



Image::Image(QWidget *parent, const char *name,std::string filename,bool silent)
  :QWidget(parent),pm(),pmScaled()
{
    this->setWindowTitle(name);
myimageptr=0;
mymspec=0;
silence=silent;
//get one from a DM file
//this may take some time...a wait cursor is initiated in LoadDM3

try{
  LoadDM3(filename);
}
catch(ImageErr::load_cancelled()){
  throw ImageErr::load_cancelled();//cancel the creation of this object
  }
catch(ImageErr::load_error& e){
  if (!silence) Saysomething mysay(0,"Error",e.msgptr);
  throw ImageErr::load_cancelled();//cancel the creation of this object
  }
catch(...){
  throw ImageErr::load_cancelled();//cancel the creation of this object
}

if (myimageptr!=0){
  //adapt the size of this widget to fit around the image
  this->setFixedSize (myimageptr->width(),myimageptr->height());
  //create a pixmap from it
  this->reconvertImage();
  //set the caption
  this->setWindowTitle(getname().c_str());
  QApplication::restoreOverrideCursor(); // we're done
  }
}

Image::~Image(){
if (myimageptr!=0) delete(myimageptr);
}

void Image::paintEvent( QPaintEvent * e)
{
QPainter painter(this);
    if ( pm.size() != QSize( 0, 0 ) ) {         // is an image loaded?
        painter.setClipRect(e->rect());
        //painter.drawPixmap(0,0,pmScaled);
        painter.drawPixmap(0,0,pm);
    }
}
bool Image::reconvertImage()
{
    bool success = FALSE;
    if ( myimageptr->isNull() ) return FALSE;
    pm.fromImage(*myimageptr);

        pmScaled = QPixmap();
        scale();
        /*resize( width(), height() );*/
        success = TRUE;                         // load successful

    return success;                             // TRUE if loaded OK
}
void Image::scale()
{

    int h = height();
    if ( myimageptr->isNull() ) return;
    if ( width() == pm.width() && h == pm.height() )
    {                                           // no need to scale if widget
        pmScaled = pm;                          // size equals pixmap size
    } else {
            QMatrix m;                         // transformation matrix
            m.scale(((double)width())/pm.width(),// define scale factors
                    ((double)h)/pm.height());
            pmScaled = pm.scaled ( width(), height());           // create scaled pixmap

    }
}

void Image::LoadDM3(std::string filename){
if (filename==""){
  //open a DM3 image
  //start to get a filename to load
  Getfilename myfile(0,"Select a DM file to load");
    //myfile.setdirname("~");
  myfile.setfilter("Projects (*.dm3 *.DM3)");
  myfile.setcaption("Select a DM file to load");
  try{
    filename=myfile.open();
  }
  catch(...){
    throw ImageErr::load_cancelled();//cancelled by user
    }
  if (filename=="") throw ImageErr::load_cancelled();//cancelled by user
}

  #ifdef DEBUG_IMAGE
  std::cout <<"the filename is: "<<filename <<"\n";
  #endif
   this->setname(filename);

   QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
  //try to open for reading to see if it exists
   std::ifstream dmfile(filename.c_str(),std::ios::in|std::ios::binary);
  if (!dmfile) {
        throw ImageErr::load_error("Can not open file for reading");
      }


  //try to read the TOP SECRET DM 3 file structure
  //DM 3 needed a special file format to add tags to images
  //you wonder why people need TIFF ( t-a-g-g-e-d image format)
  //if GATAN is helping us out ;(
  //but anyway, we are stuck with it now...so let's open it

  Multibyte i4=Multibyte(4);
  Multibyte i4r=Multibyte(4,true); //reversed order 4byte
  Multibyte i2=Multibyte(2);
  Multibyte i2r=Multibyte(2,true); //reversed order 2byte
//test
 // while (1!=0){ //tag=0 means end of file
 //  i2.get(dmfile);//length of name of tag
//	i2.debug();
 // }
  //test
/*
  //test floating point numbers
  Multibyte f4=Multibyte(4);

  //code the numer 9.75
  f4.setbyte(0,65);
  f4.setbyte(1,28);
  f4.setbyte(2,0);
  f4.setbyte(3,0);
  #ifdef DEBUG_IMAGE
  f4.debug();
  std::cout<<"the number in float is="<<f4.getfloatvalue()<<"\n";
  #endif
  f4.setbyte(0,193); //different sign
  #ifdef DEBUG_IMAGE
  f4.debug();
  std::cout<<"the number in float is="<<f4.getfloatvalue()<<"\n";
  #endif
  f4.setbyte(1,156); //flip the last bit of the exponent
  #ifdef DEBUG_IMAGE
  f4.debug();
  std::cout<<"the number in float is="<<f4.getfloatvalue()<<"\n";
  #endif
  f4.setbyte(0,65);
  #ifdef DEBUG_IMAGE
  f4.debug();
  std::cout<<"the number in float is="<<f4.getfloatvalue()<<"\n";
  #endif

  //test the multibyte functionality
  i4.setbyte(0,255);
  i4.setbyte(1,128);
  i4.setbyte(2,4);
  i4.setbyte(3,0);
  i4.debug(); //print some info

  i4r.setbyte(0,255);
  i4r.setbyte(1,128);
  i4r.setbyte(2,4);
  i4r.setbyte(3,0);
  i4r.debug(); //print some info

*/


  //start reading header byte by byte
  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //3
  #endif
  if (i4.getvalue()!=3) throw ImageErr::load_error("Not a DM3 file (1st header i4!=3)");

  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //filelength-16
  #endif
  const int filelength=i4.getvalue()-16;
  if (filelength<=0) throw ImageErr::load_error("Not a DM3 file (2nd header filelength<=0)");

  i4.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4.debug(); //byte order
  #endif
  if (i4.getvalue()>1) throw ImageErr::load_error("Not a DM3 file (3rd header byte order>1)");
  const bool byteorder=i4.getvalue();



  i4r.get(dmfile);
  #ifdef DEBUG_IMAGE_DETAIL
  i4r.debug(); //1?
  #endif
  if (i4r.getvalue()!=1) throw ImageErr::load_error("Not a DM3 file (4rd header !=1)");


  i2.get(dmfile);
  i2.debug(); //number of tags
  //const int ntags=i2.getvalue();

  //create a list with tags during reading
  std::vector<Tag*> taglist;

  //start reading the tags
  char tag=1;

  while (tag!=0){ //tag=0 means end of file
    dmfile.get(tag);
    //read name of tag
    i2.get(dmfile);//length of name of tag
    const int ltname=i2.getvalue();
    #ifdef DEBUG_IMAGE_DETAIL
    std::cout<<"length of tagname:"<<ltname<<"\n";
    i2.debug();
    #endif

    if (ltname>LTMAX) throw ImageErr::load_error("An unreasonable long tag name encountered");
    char tname[LTMAX];
    //tname="";
    if (ltname>0) {
      dmfile.get(tname,ltname+1,0); //read name of the tag, make sure we don't stop for eol
    }
    else{
      strcpy(tname,"");
    }
    #ifdef DEBUG_IMAGE_DETAIL
    std::cout<<"tagname:"<<tname<<"\n";
    #endif


    //then depending on type of tag do
    switch (tag){
      case 21:{
          //a normal tag

          //read four % signs
          char c[4];
          dmfile.read(c,4); //read 4 chars and check wehter they are % signs
          if ((c[0]!='%')||(c[1]!='%')||(c[2]!='%')||(c[3]!='%')) throw ImageErr::load_error("Not a DM3 file:A tag should start with %%%%");

          i4.get(dmfile); //number of info numbers following not including data
          //const int nnum=i4.getvalue();
            i4.get(dmfile);
            const int type=i4.getvalue();
              switch(type){
                case 1:
                case 2:
                case 3:
                case 4:
                case 5:
                case 6:
                case 7:
                case 8:
                case 9:
                case 10:
                case 15:{
                  //read the data of type type
                  readtype(dmfile,type,byteorder,tname,taglist,false,true);
                  break;
                  }
                case 20:{
                //multiple data or groups
                  i4.get(dmfile); //get type
                  const int subtype=i4.getvalue();
                  readtype(dmfile,subtype,byteorder,tname,taglist,true,true);
                break;
                }

              default:{
                 throw ImageErr::load_error("Not a DM3 format?: An unknown info type number was encountered");
                }
              }
        break; //of the case 21 switch
      }
      case 20:{
          //a directory of tags
          i4r.get(dmfile);
          i2.get(dmfile); //the number of tags in this directory
          #ifdef DEBUG_IMAGE_DETAIL
          std::cout<<"a tag directory\n";
          i4r.debug();
          i2.debug();
          #endif
          //create a tag with name and number of groups
          Tag* thistag=new Tag(tname,1,Tag::tagi2);
          thistag->seti2(0,i2.getvalue());
          taglist.push_back(thistag);
          break;
      }

       case 0:{
          //end of the file
          #ifdef DEBUG_IMAGE_DETAIL
          std::cout<<"reached the end\n";
          #endif
          break;
      }

      default:{
        throw ImageErr::load_error("Reached an unexpected tag identifier");
      }

    }
 #ifdef DEBUG_IMAGE_DETAIL
      //print the current taglist
      std::cout<<"The current tag list \n";
      std::cout<<"*********************\n";
      for (unsigned int j=0;j<taglist.size();j++){
        std::cout<<taglist[j]->getname()<<"\n";
      }
      std::cout<<"*********************\n";
      #endif
  }

//print a full report on the tags
  #ifdef DEBUG_IMAGE_DETAIL
      //print the current taglist
      std::cout<<"The current detailed tag list \n";
      std::cout<<"******************************\n";
      for (unsigned int j=0;j<taglist.size();j++){
        std::cout<<taglist[j]->getname()<<"\n";
        if (taglist[j]->getsize()<16){
           for (unsigned int index=0;index<taglist[j]->getsize();index++){
            if (taglist[j]->isint()) {
               int value=taglist[j]->getint(index);
               std::cout<<value;
               if (isalnum(char(value))) {
                  std::cout<<" - "<<char(value);
                }
               std::cout<<"\n";
            }
            if (taglist[j]->isreal()) std::cout<<taglist[j]->getreal(index)<<"\n";
            if (taglist[j]->ischar()) std::cout<<taglist[j]->gettagchar(index)<<"\n";
            }
           }
      }
      std::cout<<"******************************\n";
      #endif

//now read from the taglist and convert to what we really want to know
//in general there is more than one image in the file
//check how many times we have a data tag
//and store the index in imlist
std::vector<unsigned int> imlist;
for (unsigned int j=0;j<taglist.size();j++){
        if (taglist[j]->getname()=="Data") imlist.push_back(j);
      }
//now look which one has the largest dimensions
std::vector<unsigned int> widthlist;
std::vector<unsigned int> heightlist;
std::vector<unsigned int> depthlist;
std::vector<unsigned int> zdimlist;


double energyscale=1.0;
double energyoffset=0.0;

for (unsigned int image=0;image<imlist.size();image++){
unsigned int theend=0;
if ((image+1)<imlist.size()) theend=imlist[image+1];
else theend=taglist.size();
unsigned int width=0;
unsigned int height=0;
unsigned int depth=0;
unsigned int zdim=0;
   for (unsigned int j=imlist[image];j<theend;j++){
    //look in the region AFTER the data tag but before the next data tag
     if (taglist[j]->getname()=="Dimensions"){
       const unsigned int dim=taglist[j]->getint(0);
       width=taglist[j+1]->getint(0);
       //default values
       height=1;
       zdim=0;
       if (dim>1){
           //get 2nd dimension
        height=taglist[j+2]->getint(0);
       }
       if (dim>2){
           //get 3rd dimension
        zdim=taglist[j+3]->getint(0); //see if we have a 3rd dimension
       }
     }
      if (taglist[j]->getname()=="PixelDepth"){
       depth=taglist[j]->getint(0);
       }
      if (taglist[j]->getname()=="Units"){
          int first=0;
          int second=0;
        try{
          first=taglist[j]->getint(0);
          second=taglist[j]->getint(1);
        }
        catch(...){
          //it didn't work this means there is no "eV" here, just skip it
        }
        if ((first==101)&&(second==86)){
          //an eV unit found
          //extract energy scale and offset
          try{
            energyscale=taglist[j-1]->getreal(0);
            energyoffset=-taglist[j-2]->getreal(0);   //stored as neg number in DM, and in pixels, not in eV
            energyoffset*=energyscale; //convert to eV
            
            std::cout<<"The energyoffset is:"<<energyoffset<<"\n";
            std::cout<<"The energyscale is:"<<energyscale<<"\n";
          }
          catch(...){
            throw ImageErr::load_error("error trying to get energyscale and offset");
          }
        }
       }
    }
   widthlist.push_back(width);
   heightlist.push_back(height);

   zdimlist.push_back(zdim);
   depthlist.push_back(depth);
}

#ifdef DEBUG_IMAGE
//print this list of info
for (unsigned int i=0;i<imlist.size();i++){
std::cout<<"Image:"<<i<<" has dimensions "<<widthlist[i]<<" x "<<heightlist[i]<<" x "<<zdimlist[i]<<" and pixeldepth "<<depthlist[i];
//print the datatype
if (taglist[(imlist[i])]->isint()) std::cout<<" the datatype is int\n";
if (taglist[(imlist[i])]->isreal()) std::cout<<" the datatype is double\n";
if (taglist[(imlist[i])]->ischar()) std::cout<<" the datatype is char\n";

std::cout<<"The energyoffset is:"<<energyoffset<<"\n";
std::cout<<"The energyscale is:"<<energyscale<<"\n";
}
#endif


unsigned int maxwidth=0;
unsigned int currimage=1;
//image 0 is a display image, image 1 is the true data
if (currimage>imlist.size()){
    currimage=0; //only take zero if there is really no other image
}

//for (unsigned int i=0;i<imlist.size();i++){
//if (widthlist[i]>maxwidth){
//  currimage=i;
//  maxwidth=widthlist[i];
//  }
//}



unsigned int width=widthlist[currimage];
unsigned int height=heightlist[currimage];
const unsigned int zdepth=zdimlist[currimage];
unsigned int pixeldepth=depthlist[currimage];
const int max=int(taglist[(imlist[currimage])]->getmax());
const int min=int(taglist[(imlist[currimage])]->getmin());

//allowed pixel depth for Qt is 1,8,32
if (pixeldepth==1){
  pixeldepth=8;  //8bpp
  }
else{
     pixeldepth=32;
     }

#ifdef DEBUG_IMAGE
std::cout<<"The image size is:"<<width<<"x"<<height<<"x"<<zdepth<<"\n";
std::cout<<"The pixel depth is:"<<pixeldepth<<" bpp \n";
std::cout<<"The maximum pixelvalue is:"<<max<<"\n";
std::cout<<"The minimum pixelvalue is:"<<min<<"\n";
#endif




//copy the data in the image
unsigned int newwidth;
unsigned int newheight;
unsigned int epoints;
unsigned int nrofspectra;
bool flip=false;
b2D=false;
//in case of 3D image, convert into 2D
if (zdepth!=0){

    #ifdef CONVERT2D
    Saysomething mysay(0,"Info","Converting 3D SI to 2D");
    newheight=width*height; //fold 2D image in one long line
    newwidth=zdepth;
    epoints=zdepth;
    nrofspectra=width*height;
    //and flip the dimensions
    flip=true;
    #endif
    #ifndef CONVERT2D
    //alternative
    newheight=height;
    newwidth=width;
    epoints=zdepth;
    nrofspectra=width*height;
    b2D=true;
    #endif
}
else{
    newheight=height;
    newwidth=width;
    epoints=width;
    nrofspectra=height;
}

//create the image
if (pixeldepth<=8){
  //an image with a palette
  //myimageptr=new QImage(newwidth,newheight,pixeldepth,int(pow(2.0,pixeldepth)));
  myimageptr=new QImage ( newwidth,newheight, QImage::Format_Indexed8 ) ;







  for (unsigned int x=0;x<newwidth;x++){
    for (unsigned int y=0;y<newheight;y++){
      //adjust the display value to fit into the range 0-255 in BW mode
      unsigned int index=0;
      if (!flip){
        index=x+y*(newwidth);
      }
      else{
        index=y+x*(newheight);
      }
      double color=taglist[(imlist[currimage])]->getvalue(index);
      color=((color-min)/(max-min))*255.0;
      int icolor=int(color);
      myimageptr->setPixel(x,y,icolor);
      }
    }
  }
else{
//an image with rgb pixels
//create the image
//myimageptr=new QImage(newwidth,newheight,pixeldepth,0);
myimageptr=new QImage ( newwidth,newheight,QImage::Format_RGB32 );
  for (unsigned int x=0;x<newwidth;x++){
    for (unsigned int y=0;y<newheight;y++){
      //adjust the display value to fit into the range 0-255 in BW mode
      unsigned int index=0;
      if (!flip){
        index=x+y*(newwidth);
      }
      else{
        index=y+x*(newheight);
      }
      double color=taglist[(imlist[currimage])]->getvalue(index);
      color=((color-min)/(max-min))*255.0;
      int icolor=int(color);
      QColor rgbcolor=QColor(icolor,icolor,icolor);
      myimageptr->setPixel(x,y,rgbcolor.rgb());
      }
    }
  }


#ifdef DEBUG_IMAGE
std::cout << "about to copy data into spectra\n";
#endif

//copy the data into a multispectrum
//energy loss axis is assumed to be x axis
mymspec=new Multispectrum();
mymspec->setfilename(filename);
mymspec->set2D(b2D);
if (b2D){
    mymspec->setstride(newwidth);
}

for (unsigned int y=0;y<nrofspectra;y++){
  //create a new spectrum for each y value
  Spectrum* currentspec=new Spectrum(epoints,energyoffset,energyscale);
  //create a name for this slice
  std::string result=filename;
  if (b2D){
    char indexstr1[10];
    const int id1=y/newwidth;
    sprintf(indexstr1,"%d",id1);
    char indexstr2[10];
    const int id2=y-id1*newwidth;
    sprintf(indexstr2,"%d",id2);
    result=result+" ["+indexstr1+','+indexstr2+']';
  }
  else{
    char indexstr[10];
    sprintf(indexstr,"%d",y);
    result=result+" ["+indexstr+']';
  }
  currentspec->setname(result);
  //copy the spectrum into the currentspec
    for (unsigned int x=0;x<epoints;x++){
      unsigned int index=0;
      if (!flip){
        if (b2D){
            index=y+x*(nrofspectra);
          }
        else{
             index=x+width*y;
        }
      }
      else{
        index=x*height+y;
      }
      double cts=taglist[(imlist[currimage])]->getvalue(index);
      currentspec->setcounts(x,cts);
    }
    currentspec->setpppc(1.0);
    currentspec->initpoissonerror();
    //add this spectrum to the multispectrum
    mymspec->addspectrum(currentspec);
    }
mymspec->setname(getname());

}


void Image::LoadDM2_5(){
}
std::string Image::getname()const{
return name;
}
void Image::setname(std::string namestr){
name=namestr;
}
unsigned int Image::getwidth()const{
return myimageptr->width();
}
unsigned int Image::getheight()const{
return myimageptr->height();
}
Multispectrum* Image::getmspec()const{
return mymspec;
}


void Image::readtype(std::ifstream& dmfile,int type,bool byteorder,std::string tagname,std::vector<Tag*>& taglist,bool inside14,bool newtag){

 //return a string with the read data of type
  std::string valuestring;
  //do this info(nnum) times in case this is inside a type 14h tag
  int ngroups=0;

  if ((inside14)&&(type!=15)){
    Multibyte i4=Multibyte(4);
    i4.get(dmfile); //should be zero
    ngroups=i4.getvalue();
  }
  else{
    ngroups=1;
  }
  #ifdef DEBUG_IMAGE_DETAIL
  std::cout<<"reading "<<ngroups<<" groups\n";
  #endif
 switch (type){
              case 2:{
                //i2* signed
                Multibyte i2=Multibyte(2);
                Multibyte i2r=Multibyte(2,true);
                int value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi2);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }

                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    i2r.get(dmfile);
                    value=i2r.getsignedvalue();
                    }
                  else {
                    i2.get(dmfile);
                    value=i2.getsignedvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a i2* signed info number with value:"<<value<<"\n";
                  i2.debug();
                  i2r.debug();
                  #endif
                  thistag->seti2(g,value);
                  }
                break;
                }
               case 3:{
                //i4* signed
                Multibyte i4=Multibyte(4);
                Multibyte i4r=Multibyte(4,true);
                long value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi4);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    i4r.get(dmfile);
                    value=i4r.getsignedvalue();
                    }
                  else {
                    i4.get(dmfile);
                    value=i4.getsignedvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a i4* signed info number with value:"<<value<<"\n";
                  i4.debug();
                  i4r.debug();
                  #endif
                  thistag->seti4(g,value);
                  }
                break;
                }

                case 4:{
                //i2* unsigned
                Multibyte i2=Multibyte(2);
                Multibyte i2r=Multibyte(2,true);
                int value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi2);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    i2r.get(dmfile);
                    value=i2r.getvalue();
                    }
                  else {
                    i2.get(dmfile);
                    value=i2.getvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a i2* unsigned info number with value:"<<value<<"\n";
                  i2.debug();
                  i2r.debug();
                  #endif
                  thistag->seti4(g,value); //may not fit into an signed int!
                  }
                break;
                }

                case 5:{
                //i4* unsigned
                Multibyte i4=Multibyte(4);
                Multibyte i4r=Multibyte(4,true);
                long value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi4);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    i4r.get(dmfile);
                    value=i4r.getvalue();
                    }
                  else {
                    i4.get(dmfile);
                    value=i4.getvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a i4* unsigned info number with value:"<<value<<"\n";
                  i4.debug();
                  i4r.debug();
                  #endif
                  thistag->seti4(g,value);
                  }
                break;
                }

                case 6:{
                //f4*
                Multibyte f4=Multibyte(4);
                Multibyte f4r=Multibyte(4,true);
                double value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagf4);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    f4r.get(dmfile);
                    value=f4r.getfloatvalue();
                    }
                  else {
                    f4.get(dmfile);
                    value=f4.getfloatvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a f4* info number with value:"<<value<<"\n";
                  f4.debug();
                  f4r.debug();
                  #endif
                  thistag->setf4(g,value);
                  }
                break;
                }

                case 7:{
                //f8*
                Multibyte f8=Multibyte(8);
                Multibyte f8r=Multibyte(8,true);
                double value=0;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagf8);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  if (byteorder){
                    f8r.get(dmfile);
                    value=f8r.getfloatvalue();
                    }
                  else {
                    f8.get(dmfile);
                    value=f8.getfloatvalue();
                    }
                  #ifdef DEBUG_detail
                  std::cout<<"a f8* info number with value:"<<value<<"\n";
                  f8.debug();
                  f8r.debug();
                  #endif
                  thistag->setf8(g,value);
                  }
                break;
                }

                case 8:{
                //i1
                Multibyte i1=Multibyte(1);
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi1);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  i1.get(dmfile);
                  const int value=i1.getvalue();
                  #ifdef DEBUG_detail
                  std::cout<<"a i1 info number with value:"<<value<<"\n";
                  i1.debug();
                  #endif
                  thistag->seti1(g,value);
                  }
                break;
                }

                case 9:{
                //a1
                char a1;
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::taga1);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  dmfile.get(a1);
                  #ifdef DEBUG_detail
                  std::cout<<"a a1 info number with value:"<<a1<<"\n";
                  #endif
                  thistag->seta1(g,a1);
                  }
                break;
                }

                case 10:{
                //i1
                Multibyte i1=Multibyte(1);
                //create a new tag and fill it out if inside tag14 only add to the current tag
                Tag* thistag;
                if (newtag) {
                  thistag=new Tag(tagname,ngroups,Tag::tagi1);
                  taglist.push_back(thistag);
                  }
                else {
                  thistag=taglist.back(); //take the last one
                  }
                for (int g=0;g<ngroups;g++){
                  i1.get(dmfile);
                  const int value=i1.getvalue();
                  #ifdef DEBUG_detail
                  std::cout<<"a i1 info number with value:"<<value<<"\n";
                  i1.debug();
                  #endif
                  thistag->seti1(g,value);
                  }
                break;
                }


                case 15:{
                //group of data
                Multibyte i4=Multibyte(4);
                i4.get(dmfile); //should be zero
                if (i4.getvalue()!=0) throw ImageErr::load_error("Not a DM3 file:group of data should start with 0");
                i4.get(dmfile); //number in the group
                const int n=i4.getvalue();
                #ifdef DEBUG_detail
                std::cout<<"a group with:"<<n<<"subtags\n";
                #endif
                std::vector<int> subtype(n);
                for (int i=0;i<n;i++){
                  i4.get(dmfile); //should be zero
                  if (i4.getvalue()!=0) throw ImageErr::load_error("Not a DM3 file:a subtag should start with 0");
                  i4.get(dmfile); //data type
                  subtype[i]=i4.getvalue();
                  #ifdef DEBUG_detail
                  std::cout<<"     member["<<i<<"] has data type"<<subtype[i]<<"\n";
                  #endif
                  }
                  int ngroups=0;
                  if (inside14){
                    Multibyte i4=Multibyte(4);
                    i4.get(dmfile); //should be zero
                    ngroups=i4.getvalue();
                    }
                  else{
                    ngroups=1;
                  }
                  bool createnewtag=true;
                  for (int g=0;g<ngroups;g++){
                    //get the data
                    for (int i=0;i<n;i++){
                      //recursive call!!!
                      readtype(dmfile,subtype[i],byteorder,tagname,taglist,false,createnewtag);
                      createnewtag=false;
                    }
                  }
                break;
                }

                default:{
                  throw ImageErr::load_error("Unknown type");
                }

            }
}
