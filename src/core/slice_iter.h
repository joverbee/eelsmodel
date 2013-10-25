/***************************************************************************
                          slice_iter.h  -  description
                             -------------------
    begin                : Sat Nov 16 2002
    copyright            : (C) 2002 by Jo Verbeeck
    email                : jo@localhost.localdomain
 ***************************************************************************/

//adapted from Stroustrups book, the c++ programming language
 
/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SLICE_ITER_H
#define SLICE_ITER_H

#include<valarray>

/* needed for matrix class */

template<class T> class Slice_iter{
  std::valarray<T>* v;  //pointer to the valarray this slice is pointing to
  std::slice s;
  size_t curr;          //index of current element
  T& ref(size_t i)const{return (*v)[s.start()+i*s.stride()];}
public:
Slice_iter(std::valarray<T>* vv,std::slice ss)
   :v(vv),s(ss),curr(0)
    {
    }
//~Slice_iter(){}
Slice_iter end(){
    Slice_iter t=*this;
    t.curr=s.size();//index of element after last element
    return t;
    }
Slice_iter start(){
    curr=0;
    return *this;    
    }
size_t size(){
  return s.size();  
}
  Slice_iter& operator++(){curr++;return *this;}
  Slice_iter operator++(int){Slice_iter t=*this;curr++;return t;}
  T& operator[](size_t i){return ref(curr=i);} //subscription in C style
  T& operator*(){return ref(curr);} //reference to current element
/*  bool operator==(const Slice_iter<T>& p,const Slice_iter<T>& q){
    return p.curr==q.curr && P.s.stride()==q.s.stride() && p.s.start()==q.s.start();
    }
  bool operator!=(const Slice_iter<T>& p,const Slice_iter<T>& q){
    return !(p==q);
    }
  bool operator<(const Slice_iter<T>& p,const Slice_iter<T>& q){
    return p.curr<q.curr && P.s.stride()==q.s.stride() && p.s.start()==q.s.start();
    }
*/
};

#endif
