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
 * eelsmodel - core/cslice_iter.h
 **/

#ifndef CSLICE_ITER_H
#define CSLICE_ITER_H

#include<valarray>

/* needed for matrix class */

template<class T> class Cslice_iter{
  std::valarray<T>* v;  //pointer to the valarray this slice is pointing to
  std::slice s;
  size_t curr;          //index of current element
  T& ref(size_t i)const{return (*v)[s.start()+i*s.stride()];}
public:
Cslice_iter(std::valarray<T>* vv,std::slice ss)
   :v(vv),s(ss),curr(0)
    {
    }
//~Cslice_iter(){}
Cslice_iter end(){
    Cslice_iter t=*this;
    t.curr=s.size();//index of element after last element
    return t;
    }
Cslice_iter start(){
    curr=0;
    return *this;
    }
size_t size(){
  return s.size();
}
  Cslice_iter& operator++(){curr++;return *this;}
  Cslice_iter operator++(int){Cslice_iter t=*this;curr++;return t;}
  T& operator[](size_t i){return ref(curr=i);} //subscription in C style
  T& operator*(){return ref(curr);} //reference to current element
/*  bool operator==(const Cslice_iter<T>& p,const Cslice_iter<T>& q){
    return p.curr==q.curr && P.s.stride()==q.s.stride() && p.s.start()==q.s.start();
    }
  bool operator!=(const Cslice_iter<T>& p,const Cslice_iter<T>& q){
    return !(p==q);
    }
  bool operator<(const Cslice_iter<T>& p,const Cslice_iter<T>& q){
    return p.curr<q.curr && P.s.stride()==q.s.stride() && p.s.start()==q.s.start();
    }
*/
};

#endif
