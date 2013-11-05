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
 * eelsmodel - core/multibyte.h
 **/

#ifndef MULTIBYTE_H
#define MULTIBYTE_H

#include <iostream>
#include <vector>

/* A class to represent multi-byte variables, with possibilities for little endian, big endian and reversed order */

class Multibyte {
std::vector<unsigned char> bytestring;
bool order; //false=normal order (1st byte is MSB), true is reversed
public:
Multibyte(unsigned int n); //create a multibyte with n bytes
Multibyte(unsigned int n,bool rev);

~Multibyte();
void setbyte(unsigned int index,unsigned char byte); //enter a byte at position index
unsigned char getbyte(unsigned int index)const;
unsigned char getbyte(unsigned int index,bool rev)const; //get a byte with index either in reversed or normal order
unsigned int getvalue();
int getsignedvalue();
double getfloatvalue();
bool indexOK(unsigned int index)const;
void debug();
unsigned int getsize()const{return bytestring.size();};
std::ostream& put(std::ostream& os);
std::istream& get(std::istream& is);
};
//special io operators for this class


#endif
