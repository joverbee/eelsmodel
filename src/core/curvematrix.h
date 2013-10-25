/***************************************************************************
                          curvematrix.h  -  description
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

#ifndef CURVEMATRIX_H
#define CURVEMATRIX_H

#include <iostream>

//use the boost library for matrices, faster because bindings to ATLAS
#include <boost/numeric/bindings/atlas/clapack.hpp>
#include <boost/numeric/bindings/traits/ublas_matrix.hpp>
#include <boost/numeric/bindings/traits/std_vector.hpp>
#include <boost/numeric/ublas/io.hpp>
#include <boost/numeric/bindings/lapack/geqrf.hpp>
#include <boost/numeric/bindings/lapack/ormqr.hpp>

namespace ublas = boost::numeric::ublas;
namespace atlas = boost::numeric::bindings::atlas;
namespace lapack = boost::numeric::bindings::lapack;

class CurveMatrix {
//member data
private:
ublas::matrix<double,ublas::column_major> M; //use column major for faster linking to ATLAS, init to 100x100 as a start, can easily be changed later
std::vector<int> ipiv;   // pivot vector, needed for fast matrix inversion

public:
//constructors and destructors
CurveMatrix();
CurveMatrix(size_t n);            //create an nxn matrix initialised with zeros
CurveMatrix(size_t n1,size_t n2); //create an n1xn2 matrix initialised with zeros
CurveMatrix(const CurveMatrix& A); //create an n1xn2 matrix initialised with A

~CurveMatrix();

//public inspectors
size_t dim1()const{return M.size1();}      //get dimension 1
size_t dim2()const{return M.size2();}      //get dimension 2
void debugdisplay()const;           //print some debug info to stdout
bool square()const{return (M.size1()==M.size2());}     //check if square matrix
void unit(); //create unit matrix if square
//public accessors
void inv_inplace();                 //in place inversion
void inv_inplace_ATLAS();                 //in place inversion, faster
void inv_inplace_slow();

void resize(size_t n);              //resize to nxn matrix, initialises all elements to zero
void resize(size_t n1,size_t n2);   //resize to n1xn2 matrix, initialises all elements to zero
void clear();                       //put all elements to zero
void clearupper();                  //clear upper right corner including diagonal
void clearlower();                  //clear lower left corner including diagonal
double getmin()const;
double getmax()const;

CurveMatrix& operator=(const CurveMatrix& A);
void multiply(const CurveMatrix& A,const CurveMatrix& B);
void multiply_slow(const CurveMatrix& A,const CurveMatrix& B);
void multiply_ATLAS(const CurveMatrix& A,const CurveMatrix& B);//faster matrix multiply
void QRdecomp(CurveMatrix& Q,CurveMatrix& R)const;

size_t rows()const;
size_t cols()const;

double operator()(size_t i, size_t j)const;
double& operator()(size_t i, size_t j);



const ublas::matrix<double,ublas::column_major>& getmatrix()const{return M;}
ublas::matrix<double,ublas::column_major>& getmatrix(){return M;}
//std::vector<double>& getipiv(){return ipiv;}
//const std::vector<double>& getipiv()const{return ipiv;}

void transpose(const CurveMatrix & A);
//exeptions
public: class CurveMatrixerr{
    public:
    class bad_alloc{};
    class not_square{};
    };
};


#endif
