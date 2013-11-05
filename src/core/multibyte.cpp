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
 * eelsmodel - core/multibyte.cpp
 **/

#include "src/core/multibyte.h"

#include <fstream>
#include <iostream>
#include <cmath>

Multibyte::Multibyte(unsigned int n)
:bytestring(n)
{
order=false; //normal order
}

Multibyte::Multibyte(unsigned int n,bool rev)
:bytestring(n)
{
order=rev;
}

Multibyte::~Multibyte(){
}

void Multibyte::setbyte(unsigned int index,unsigned char byte){
//enter a byte at position index
  if (indexOK(index)) bytestring[index]=byte;
}

unsigned char Multibyte::getbyte(unsigned int index)const{
  if (indexOK(index)) return bytestring[index];
  else return 0;
}

unsigned char Multibyte::getbyte(unsigned int index,bool rev)const{
  //get byte with index
  //interpret index as normal rev=false or as reversed byte order when rev=true
  if (!rev){
    if (indexOK(index)) return bytestring[index];
    else return 0;
    }
  else{
    index=bytestring.size()-index-1;
    if (indexOK(index)) return bytestring[index];
    else return 0;
    }
}
unsigned int Multibyte::getvalue(){
//get value in normal order (1st byte is MSB part)
unsigned int value=0;
for (unsigned int i=0;i<bytestring.size();i++){
   const unsigned int weight=(unsigned int)pow(256.0,bytestring.size()-i-1);
   value+=weight*(unsigned int)getbyte(i,order);
  }
return value;
}


double Multibyte::getfloatvalue(){
//get floating point value in normal order (1st byte is MSB part)
//IEEE 754 format
double value=0;
//only 4 or 8 byte mode
if (bytestring.size()==4){
  //single precision
  //determine sign
  double sign=0.0;
  int exponent;
  double mantisse;
  if (getbyte(0,order)>127) sign=-1.0;
  else sign=1.0;
  const unsigned char msbmant=(getbyte(1,order) & (unsigned char)(127));
  const unsigned char msbexp =(getbyte(0,order) & (unsigned char)(127));
  const unsigned char lsbexp =(getbyte(1,order) & (unsigned char)(128));
  exponent=int(double( double(msbexp)*2.0+ double(lsbexp)/128.0 )-127.0);
  mantisse=double(
            msbmant* pow(2.0,16)
            +getbyte(2,order)*pow(2.0,8)
            +getbyte(3,order)
            +128.0*pow(2.0,16)
            )
            /(pow(2.0,23));



  value=sign*mantisse*pow(2.0,exponent);
  }
else if (bytestring.size()==8){
  //double precision
  //determine sign
  double sign=0.0;
  int exponent;
  double mantisse;
  if (getbyte(0,order)>127) sign=-1.0;
  else sign=1.0;
  const unsigned char msbmant=(getbyte(1,order) & (unsigned char)(15));
  const unsigned char msbexp =(getbyte(0,order) & (unsigned char)(127));
  const unsigned char lsbexp =(getbyte(1,order) & (unsigned char)(240));
  exponent=int(double( double(msbexp)+ double(lsbexp)/16.0 )-1023.0);
  mantisse=double(
            msbmant* pow(2.0,56)
            +getbyte(2,order)*pow(2.0,48)
            +getbyte(3,order)*pow(2.0,40)
            +getbyte(4,order)*pow(2.0,32)
            +getbyte(5,order)*pow(2.0,16)
            +getbyte(6,order)*pow(2.0,8)
            +getbyte(7,order)
            +16.0*pow(2.0,56)
            )
            /(16.0*pow(2.0,56));
  value=sign*mantisse*pow(2.0,exponent);
  }
else{
  value=0.0;
  }
return value;
}

int Multibyte::getsignedvalue(){
//look at the MSB for the sign
const unsigned char signchar=(getbyte(0,order)& (unsigned char)(127));
int sign;
if (getbyte(0,order)>127) sign=-1;
else sign=1;
//calculate the value of the rest but invert the whole if sign is negative

int value=0;
//do the msb first
unsigned int weight=(unsigned int)pow(256.0,bytestring.size()-1);
if (sign>0) value+=weight*(unsigned int)signchar;
else value+=weight*(unsigned int)(!signchar);
for (unsigned int i=1;i<bytestring.size();i++){
   const unsigned int weight=(unsigned int)pow(256.0,bytestring.size()-i-1);
   if (sign>0)  value+=weight*(unsigned int)getbyte(i,order);
   else value+=weight*(unsigned int)(!getbyte(i,order));
  }
return sign*value; //do a conversion
}

bool Multibyte::indexOK(unsigned int index)const{
return (index<bytestring.size());
}


void Multibyte::debug(){
//print some debug info
std::cout << "the multibyte in hex: ";
for (unsigned int i=0;i<bytestring.size();i++){
    unsigned char c=getbyte(i);
    unsigned int j=(unsigned int) c;

    std::cout<<std::hex<<j<<" ";
  }
std::cout<<std::dec<<" the value is :"<<this->getvalue()<<" or hex:"<<std::hex<<this->getvalue()<<"\n";
std::cout<<std::dec; //put back in decimal mode!!!
}
std::ostream& Multibyte::put(std::ostream& os){
//output operator write n chars to output
for (unsigned int i=0;i<this->getsize();i++){
   os.put(this->getbyte(i));
  }
  return os;
}

std::istream& Multibyte::get(std::istream& is){
//read n char from input
char c[256];
if (this->getsize()>=256) return is; //unreasonable size

	//for gcc get(c) worked fine
	//for Borland bcc32 (5.5.1) get(c) skips all eol characters
	//it is not clear which one is official c++ style but Stroustrups book doesn`t say anything on eol
	//solved this by using read instead
    //char c;
    //is.get(c);

    is.read(c,this->getsize()); //read n chars

    //this->setbyte(i,c);

//copy the results
	for (unsigned int i=0;i<this->getsize();i++){
	this->setbyte(i,c[i]);
  }

return is;
}


