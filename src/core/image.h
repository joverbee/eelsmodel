/***************************************************************************
                          image.h  -  description
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

#ifndef IMAGE_H
#define IMAGE_H

//#define DEBUG_IMAGE
//#define DEBUG_detail
//#define DEBUG_IMAGE_DETAIL

#define LTMAX 256

#include <vector>
#include <fstream>
#include <string>

#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QWidget>

class Tag;
class Multispectrum;

/* class to show 2D data as images, same as graph for 1D */

class Image : public QWidget  {

Q_OBJECT
QImage* myimageptr;
QPixmap pm;         //the converted pixmap
QPixmap pmScaled;   // the scaled pixmap
std::string name;
Multispectrum* mymspec;
bool b2D;
bool silence; //wether or not to show errors on screen
public:
class ImageErr{   //class for exeptions
    class bad_alloc{};
    public:class load_error{ public: const char* msgptr;load_error(const char* e){msgptr=e;}};
    class load_cancelled{};
    };
public:
//constructor and destructor
Image(QWidget *parent=0, const char *name=0,std::string filename=0,bool silent=0);
~Image();
//
void LoadDM3(std::string filename);
void LoadDM2_5();
void setname(std::string namestr);
//inspectors
std::string getname()const;
unsigned int getwidth()const;
unsigned int getheight()const;
Multispectrum* getmspec()const;

protected:
void paintEvent( QPaintEvent * );
bool reconvertImage();
void scale();
bool is2D(){return b2D;}
private:
void readtype(std::ifstream& dmfile,int type,bool byteorder,std::string tagname,std::vector<Tag*>& taglist,bool inside14,bool newtag); //fill out the taglist with the read data of type
};

#endif
