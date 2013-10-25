/***************************************************************************
                          tag.h  -  description
                             -------------------
    begin                : Fri Feb 28 2003
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

#ifndef TAG_H
#define TAG_H

#include <string>
#include <vector>

/* A tag to hold information for the DM3 image file format. A tag has a name and data attached to it. */

class Tag {

enum typeident{CHARTYPE,FLOATTYPE,INTTYPE};
typeident tagtype;
std::string tagname;
std::vector<long> intvector;
std::vector<double> doublevector;
std::vector<char> charvector;
public:
enum tagid{tagi1,tagi2,tagi4,tagf4,tagf8,taga1};
class TagErr{   //class for exeptions
    public:
    class bad_alloc{};
    class bad_index{};
    };

public:
Tag(std::string name,unsigned int size,tagid type);

~Tag();
std::string getname()const;
unsigned int getsize()const;
long getint(unsigned int index)const;
double getreal(unsigned int index)const;
char gettagchar(unsigned int index)const;
double getvalue(unsigned int index)const;
double getmin()const;
double getmax()const;
bool isreal()const;
bool isint()const;
bool ischar()const;
bool indexOK(unsigned int index)const;

void seti1(unsigned int index,int value);
void seti2(unsigned int index,int value);
void seti4(unsigned int index,long value);
void setf4(unsigned int index,double value);
void setf8(unsigned int index,double value);
void seta1(unsigned int index,char value);

};

#endif
