/***************************************************************************
                          tag.cpp  -  description
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

#include "src/core/tag.h"

Tag::Tag(std::string name,unsigned int size,tagid type)
:intvector(),doublevector(),charvector()
{
tagname=name;
switch(type){
  case tagi1:{
    tagtype=INTTYPE;
    try{
      intvector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  case tagi2:{
    tagtype=INTTYPE;
    try{
      intvector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  case tagi4:{
    tagtype=INTTYPE;
    try{
      intvector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  case tagf4:{
    tagtype=FLOATTYPE;
    try{
      doublevector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  case tagf8:{
    tagtype=FLOATTYPE;
    try{
      doublevector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  case taga1:{
    tagtype=CHARTYPE;
    try{
      charvector.resize(size);
    }
    catch(...)
    {
      throw TagErr::bad_alloc();
    }
    break;
    }
  }
}
Tag::~Tag(){
}
std::string Tag::getname()const{
return tagname;
}
unsigned int Tag::getsize()const{
switch(tagtype){
  case INTTYPE:{
    return intvector.size();
    break;
    }
  case FLOATTYPE:{
    return doublevector.size();
    break;
    }
  case CHARTYPE:{
    return charvector.size();
    break;
    }
  }
return 0;
}
double Tag::getmin()const{
  double min=getvalue(0);
  for (unsigned int i=0;i<getsize();i++){
    if (getvalue(i)<min) min=getvalue(i);
    }
  return min;
}
double Tag::getmax()const{
  double max=getvalue(0);
  for (unsigned int i=0;i<getsize();i++){
    if (getvalue(i)>max) max=getvalue(i);
    }
  return max;
}
double Tag::getvalue(unsigned int index)const{
//returns the value in float format, there might be loss of precision
switch(tagtype){
  case INTTYPE:{
    return getint(index);
    break;
    }
  case FLOATTYPE:{
    return getreal(index);
    break;
    }
  case CHARTYPE:{
    return gettagchar(index);
    break;
    }
  }
return 0;
}

long Tag::getint(unsigned int index)const{
if (indexOK(index)) return intvector[index];
else throw TagErr::bad_index();
}
char Tag::gettagchar(unsigned int index)const{
if (indexOK(index)) return charvector[index];
else throw TagErr::bad_index();
}
double Tag::getreal(unsigned int index)const{
if (indexOK(index)) return doublevector[index];
else throw TagErr::bad_index();
}
bool Tag::isreal()const{
return (tagtype==FLOATTYPE);
}
bool Tag::isint()const{
return (tagtype==INTTYPE);
}
bool Tag::ischar()const{
return (tagtype==CHARTYPE);
}
bool Tag::indexOK(unsigned int index)const{
switch(tagtype){
  case INTTYPE:{
    return (index<intvector.size());
    break;
    }
  case FLOATTYPE:{
    return (index<doublevector.size());
    break;
    }
  case CHARTYPE:{
    return (index<charvector.size());
    break;
    }
  }
return false;
}
void Tag::seti1(unsigned int index,int value){
if (indexOK(index)) intvector[index]=value;
else throw TagErr::bad_index();
}
void Tag::seti2(unsigned int index,int value){
if (indexOK(index)) intvector[index]=value;
else throw TagErr::bad_index();
}
void Tag::seti4(unsigned int index,long value){
if (indexOK(index)) intvector[index]=value;
else throw TagErr::bad_index();
}
void Tag::setf4(unsigned int index,double value){
if (indexOK(index)) doublevector[index]=value;
else throw TagErr::bad_index();
}
void Tag::setf8(unsigned int index,double value){
if (indexOK(index)) doublevector[index]=value;
else throw TagErr::bad_index();
}
void Tag::seta1(unsigned int index,char value){
if (indexOK(index)) charvector[index]=value;
else throw TagErr::bad_index();
}

