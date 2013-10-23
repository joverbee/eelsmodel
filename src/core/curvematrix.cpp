/***************************************************************************
                          curvematrix.cpp  -  description
A simple 2D matrix class to store curvature matrices based on an example in Stroustrups book
                             -------------------
    begin                : Sat Nov 16 2002
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
//#define SLOW //switch off use of ATLAS functions
#define NDEBUG
//#define CURVEMATRIXDEBUG

#include "src/core/curvematrix.h"
#include "src/core/slice_iter.h"
#include "src/core/cslice_iter.h"

#include <iostream>
#include <vector>
#include <cmath>


#include <boost/numeric/bindings/atlas/clapack.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/traits/std_vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/bindings/atlas/cblas3.hpp>
#include <boost/numeric/ublas/matrix.hpp>
#include <boost/numeric/bindings/lapack/geqrf.hpp>
#include <boost/numeric/bindings/lapack/ormqr.hpp>





//#define DEBUG_CURVEMATRIX


CurveMatrix::CurveMatrix(){
  //create an empty matrix
  try{
    M.resize (0,0,false);
    ipiv.resize(0);
  }
  catch(...){
    throw CurveMatrix::CurveMatrixerr::bad_alloc();
  }
}


CurveMatrix::CurveMatrix(size_t n){
  //create a nxn matrix
  try{
    M.resize (n,n,false);
    ipiv.resize(n);
  }
  catch(...){
    throw CurveMatrix::CurveMatrixerr::bad_alloc();
  }
}

CurveMatrix::CurveMatrix(size_t n1,size_t n2){
  //create an n1xn2 matrix initialised with zeros
  try{
    M.resize (n1,n2,false);
    ipiv.resize(n1);
  }
  catch(...){
    throw CurveMatrix::CurveMatrixerr::bad_alloc();
  }
}

CurveMatrix::CurveMatrix(const CurveMatrix& A){
    //copy constructor
    M=A.getmatrix();
    ipiv.resize(M.size1());
}

CurveMatrix& CurveMatrix::operator=(const CurveMatrix& A){
    M=A.getmatrix();
      ipiv.resize(M.size1());
    return *this;
}

double CurveMatrix::operator()(size_t i, size_t j)const{
 return M(i,j);
}
double& CurveMatrix::operator()(size_t i, size_t j){
 return M(i,j);
}


void CurveMatrix::resize(size_t n){
    //square matrix
     M.resize (n,n,false);
     ipiv.resize(n);
}

void CurveMatrix::resize(size_t n1,size_t n2){
   M.resize (n1,n2,false);
   ipiv.resize(n1);
}


CurveMatrix::~CurveMatrix(){
}



void CurveMatrix::clear(){
  M.clear();
}

void CurveMatrix::unit(){
      ublas::identity_matrix<double> eye (M.size1());
      M=eye;
}
void CurveMatrix::transpose(const CurveMatrix & A){
    this->resize(A.dim2(),A.dim1());
    M=trans(A.getmatrix());
}

void CurveMatrix::clearupper(){
  //clear the upper right corner including the diagonal
  if (!square()) throw CurveMatrixerr::not_square();
  for (size_t i=0;i<M.size1();i++){
    for (size_t j=i;j<M.size2();j++){  //is this correct???
      M(i,j)=0;
    }
  }
}

void CurveMatrix::clearlower(){
  //clear the lower left corner including the diagonal
  if (!square()) throw CurveMatrixerr::not_square();
  for (size_t i=0;i<M.size1();i++){
    for (size_t j=0;j<=i;j++){
      M(i,j)=0;
    }
  }
}
double CurveMatrix::getmin()const{
  //return the smallest value in the matrix
  double min=M(0,0);
  for (size_t i=0;i<M.size1();i++){
    for (size_t j=0;j<M.size2();j++){
      if (M(i,j)<min) min=M(i,j);
    }
  }
return min;
}

double CurveMatrix::getmax()const{
  //return the smallest value in the matrix
  double max=M(0,0);
  for (size_t i=0;i<M.size1();i++){
    for (size_t j=0;j<M.size2();j++){
      if (M(i,j)>max) max=M(i,j);
    }
  }
return max;
}

size_t CurveMatrix::rows()const{
    return M.size1();
    }
size_t CurveMatrix::cols()const{
    return M.size2();
    }


void CurveMatrix::inv_inplace(){
    #ifdef SLOW
    inv_inplace_slow();
    return;
    #endif
    
    inv_inplace_ATLAS();
}

void CurveMatrix::inv_inplace_slow(){
  //check if square
  if (!square()) throw CurveMatrixerr::not_square();
  //in place matrix inversion
  std::vector<unsigned int> ik;
  ik.resize(M.size1());
  std::vector<unsigned int> jk;
  jk.resize(M.size1());

  double amax, save;
  double det = 1.0;
  unsigned int ii=0;
  unsigned int jj=0;
  for (unsigned int k=0; k<M.size1(); k++ ){
    /* Find largest element array[i][j] in submatrix starting at k,k matrix */
    do{
      do{
        amax = 0.0;
        for (unsigned int i=k; i<M.size1(); i++ ){
          for (unsigned int j=k; j<M.size1(); j++ ){
            if (fabs(amax)<fabs(M(i,j))){
              amax = M(i,j);
              ik[k]= i;
              jk[k]= j;
            }
          }
        }
        /* Interchange rows and columns to put amax in array[k][k] */
        if ( amax == 0.0 ){
          //problem, determinant is 0, can not invert
          det = 0.0;
          return;
        }
        ii=ik[k];
      }
      while ( (int)ii-(int)k < 0 );
      if ( (int)ii-(int)k > 0 ){
        for (unsigned int j=0; j<M.size1(); j++ ){
          save =M(k,j);
          M(k,j) = M(ii,j);
          M(ii,j) = -save;
        }
      }
      jj = jk[k];
    }
    while ( (int)jj-(int)k < 0 );
    if ( (int)jj-(int)k > 0 ){
      for (unsigned int i=0; i<M.size1(); i++ ){
        save = M(i,k);
        M(i,k) = M(i,jj);
        M(i,jj) = -save;
      }
    }
    /* Accumulate elements of inverse matrix */
    for (unsigned int i=0; i<M.size1(); i++ ){
      if ( i-k != 0 ) M(i,k) = -M(i,k) / amax;
    }
    for (unsigned int i=0; i<M.size1(); i++ ){
      for (unsigned int j=0; j<M.size1(); j++ ){
        if ( i-k != 0 && j-k != 0 ) {
          M(i,j) += M(i,k)*M(k,j);
        }
      }
    }
    for (unsigned int j=0; j<M.size1(); j++ ){
      if ( j-k != 0 ){
        M(k,j) /= amax;
      }
    }
    M(k,k) = 1.0/amax;
    det = amax;
  }
  /* Restore ordering of matrix */
  for (unsigned int l=1; l<=M.size1(); l++ ){
    unsigned int k = (M.size1()) - l;
    jj = ik[k];
    if ( (int)jj-(int)k > 0 ){
      for (unsigned int i=0; i<M.size1(); i++ ){
        save = M(i,k);
        M(i,k) = -M(i,jj);
        M(i,jj) = save;
      }
    }
    ii = jk[k];
    if ( (int)ii-(int)k > 0 ){
      for (unsigned int j=0; j<M.size1(); j++ ){
        save = M(k,j);
        M(k,j) = -M(ii,j);
        M(ii,j) = save;
      }
    }
  }
}

void CurveMatrix::debugdisplay()const{
  //print some debug info
  std::cout<<"matrix debug info \n";
  std::cout<<"dimension: "<<M.size1()<<"x"<<M.size2()<<"\n";
  std::cout<<M<<std::endl;
}

void CurveMatrix::multiply(const CurveMatrix& A,const CurveMatrix& B){

    #ifdef SLOW
    multiply_slow(A,B);
    return;
    #endif
    multiply_ATLAS(A,B);
}


void CurveMatrix::multiply_slow(const CurveMatrix& A,const CurveMatrix& B){
    //Matrix multiplication
//make space for the result
    this->resize (A.dim1(),B.dim2());
    //also implement a fast multiply via ATLAS
    if (A.dim2()==B.dim1()){
        for (unsigned int i=0;i<A.dim1();i++){
            for (unsigned int j=0;j<B.dim2();j++){
                M(i,j)=0.0;
                 for (unsigned int r=0;r<A.dim2();r++){
                     const double a=A(i,r);
                     const double b=B(r,j);
                     M(i,j)+=a*b;
                 }
            }
        }

    }
    else{
        std::cout <<"wrong size of matrices in multiply A="<<A.rows()<<" x"<<A.cols()<<" B="<<B.rows()<<" x"<<B.cols()<<" this="<<this->rows()<<" x"<<this->cols()<<"\n";
    }
}

void CurveMatrix::multiply_ATLAS(const CurveMatrix& A,const CurveMatrix& B){
    //standard ublas
    //this->getmatrix = ublas::prod(A.getmatrix(),B.getmatrix());
    //do a fast ATLAS multiply
    //make space for the result
    this->resize (A.dim1(),B.dim2());

    try{
    atlas::gemm (A.getmatrix(),B.getmatrix(), this->getmatrix());  //
    }
    catch(...){
        std::cout <<"multiply_ATLAS failed....reverting to slower native implementation, check existence of ATLAS library!\n";
        multiply(A,B);
    }
}

void CurveMatrix::inv_inplace_ATLAS(){
    //a fast implementation using ATLAS
    //this may crash if the library is not found or if it is compiled for the wrong machine in that case revert to the old and slow implementation
    //ipiv already has the size of M
    if (!square()) throw CurveMatrixerr::not_square();
try{
        atlas::lu_factor (M, ipiv);  // alias for getrf()
        atlas::lu_invert (M, ipiv);  // alias for getri()
    }
    catch(...){
        std::cout <<"inv_inplace_ATLAS failed....reverting to slower native implementation, check existence of ATLAS library!\n";
        inv_inplace_slow();
    }
}

void CurveMatrix::QRdecomp(CurveMatrix& Q,CurveMatrix& R)const{
    //R=M; //copy A in R
    ublas::vector<double> vecTau( this->dim1() );
    //lapack::geqrf( R, vecTau );
    ublas::identity_matrix<double> matI( this->dim1() );
    ublas::matrix<double, ublas::column_major> matC = matI;
    //get Q from decoded version
    //lapack::ormqr( 'L', 'T', this.getmatrix(), vecTau, Q.getmatrix(), lapack::optimal_workspace());

    #ifdef CURVEMATRIXDEBUG
    std::cout <<"A=";
    this->debugdisplay();
    std::cout <<"\n";
    std::cout <<"Q=";
    Q.debugdisplay();
    std::cout <<"\n";
    std::cout <<"R=";
    R.debugdisplay();
    std::cout <<"\n";
    #endif
}

