00001 /* -*- mode: c++; c-basic-offset: 4; -*-
00002  *
00003  * Template for a diagonally banded matrix.
00004  */
00005 /*
00006  * Copyright (c) 1998,1999
00007  * University Corporation for Atmospheric Research, UCAR
00008  *
00009  * Permission to use, copy, modify, distribute and sell this software and
00010  * its documentation for any purpose is hereby granted without fee,
00011  * provided that the above copyright notice appear in all copies and that
00012  * both that copyright notice and this permission notice appear in
00013  * supporting documentation.  UCAR makes no representations about the
00014  * suitability of this software for any purpose.  It is provided "as is"
00015  * without express or implied warranty.
00016  *
00017  * Note from the author:
00018  *
00019  * Where possible, you are encouraged to follow the GNU General Public
00020  * License, or at least the spirit of the license, for the distribution and
00021  * licensing of this software and any derived works.  See
00022  * http://www.gnu.org/copyleft/gpl.html.
00023  */
00024
00025 #ifndef _BANDEDMATRIX_ID
00026 #define _BANDEDMATRIX_ID "$Id: BandedMatrix.h,v 1.14 2005/05/27 21:16:44 granger Exp $"
00027
00028 #include <vector>
00029 #include <algorithm>
00030 #include <iostream>
00031
00032 template <class T> class BandedMatrixRow;
00033
00034
00035 template <class T> class BandedMatrix
00036 {
00037 public:
00038     typedef unsigned int size_type;
00039     typedef T element_type;
00040
00041     // Create a banded matrix with the same number of bands above and below
00042     // the diagonal.
00043     BandedMatrix (int N_ = 1, int nbands_off_diagonal = 0) : bands(0)
00044     {
00045         if (! setup (N_, nbands_off_diagonal))
00046             setup ();
00047     }
00048
00049     // Create a banded matrix by naming the first and last non-zero bands,
00050     // where the diagonal is at zero, and bands below the diagonal are
00051     // negative, bands above the diagonal are positive.
00052     BandedMatrix (int N_, int first, int last) : bands(0)
00053     {
00054         if (! setup (N_, first, last))
00055             setup ();
00056     }
00057
00058     // Copy constructor
00059     BandedMatrix (const BandedMatrix &b) : bands(0)
00060     {
00061         Copy (*this, b);
00062     }
00063
00064     inline bool setup (int N_ = 1, int noff = 0)
00065     {
00066         return setup (N_, -noff, noff);
00067     }
00068
00069     bool setup (int N_, int first, int last)
00070     {
00071         // Check our limits first and make sure they make sense.
00072         // Don't change anything until we know it will work.
00073         if (first > last || N_ <= 0)
00074             return false;
00075
00076         // Need at least as many N_ as columns and as rows in the bands.
00077         if (N_ < abs(first) || N_ < abs(last))
00078             return false;
00079
00080         top = last;
00081         bot = first;
00082         N = N_;
00083         out_of_bounds = T();
00084
00085         // Finally setup the diagonal vectors
00086         nbands = last - first + 1;
00087         if (bands) delete[] bands;
00088         bands = new std::vector<T>[nbands];
00089         int i;
00090         for (i = 0; i < nbands; ++i)
00091         {
00092             // The length of each array varies with its distance from the
00093             // diagonal
00094             int len = N - (abs(bot + i));
00095             bands[i].clear ();
00096             bands[i].resize (len);
00097         }
00098         return true;
00099     }
00100
00101     BandedMatrix<T> & operator= (const BandedMatrix<T> &b)
00102     {
00103         return Copy (*this, b);
00104     }
00105
00106     BandedMatrix<T> & operator= (const T &e)
00107     {
00108         int i;
00109         for (i = 0; i < nbands; ++i)
00110         {
00111             std::fill_n (bands[i].begin(), bands[i].size(), e);
00112         }
00113         out_of_bounds = e;
00114         return (*this);
00115     }
00116
00117     ~BandedMatrix ()
00118     {
00119         if (bands)
00120             delete[] bands;
00121     }
00122
00123 private:
00124     // Return false if coordinates are out of bounds
00125     inline bool check_bounds (int i, int j, int &v, int &e) const
00126     {
00127         v = (j - i) - bot;
00128         e = (i >= j) ? j : i;
00129         return !(v < 0 || v >= nbands ||
00130                  e < 0 || (unsigned int)e >= bands[v].size());
00131     }
00132
00133     static BandedMatrix & Copy (BandedMatrix &a, const BandedMatrix &b)
00134     {
00135         if (a.bands) delete[] a.bands;
00136         a.top = b.top;
00137         a.bot = b.bot;
00138         a.N = b.N;
00139         a.out_of_bounds = b.out_of_bounds;
00140         a.nbands = a.top - a.bot + 1;
00141         a.bands = new std::vector<T>[a.nbands];
00142         int i;
00143         for (i = 0; i < a.nbands; ++i)
00144         {
00145             a.bands[i] = b.bands[i];
00146         }
00147         return a;
00148     }
00149
00150 public:
00151     T &element (int i, int j)
00152     {
00153         int v, e;
00154         if (check_bounds(i, j, v, e))
00155             return (bands[v][e]);
00156         else
00157             return out_of_bounds;
00158     }
00159
00160     const T &element (int i, int j) const
00161     {
00162         int v, e;
00163         if (check_bounds(i, j, v, e))
00164             return (bands[v][e]);
00165         else
00166             return out_of_bounds;
00167     }
00168
00169     inline T & operator() (int i, int j)
00170     {
00171         return element (i-1,j-1);
00172     }
00173
00174     inline const T & operator() (int i, int j) const
00175     {
00176         return element (i-1,j-1);
00177     }
00178
00179     size_type num_rows() const { return N; }
00180
00181     size_type num_cols() const { return N; }
00182
00183     const BandedMatrixRow<T> operator[] (int row) const
00184     {
00185         return BandedMatrixRow<T>(*this, row);
00186     }
00187
00188     BandedMatrixRow<T> operator[] (int row)
00189     {
00190         return BandedMatrixRow<T>(*this, row);
00191     }
00192
00193
00194 private:
00195
00196     int top;
00197     int bot;
00198     int nbands;
00199     std::vector<T> *bands;
00200     int N;
00201     T out_of_bounds;
00202
00203 };
00204
00205
00206 template <class T>
00207 std::ostream &operator<< (std::ostream &out, const BandedMatrix<T> &m)
00208 {
00209     unsigned int i, j;
00210     for (i = 0; i < m.num_rows(); ++i)
00211     {
00212         for (j = 0; j < m.num_cols(); ++j)
00213         {
00214             out << m.element (i, j) << " ";
00215         }
00216         out << std::endl;
00217     }
00218     return out;
00219 }
00220
00221
00222
00223 /*
00224  * Helper class for the intermediate in the [][] operation.
00225  */
00226 template <class T> class BandedMatrixRow
00227 {
00228 public:
00229     BandedMatrixRow (const BandedMatrix<T> &_m, int _row) : bm(_m), i(_row)
00230     { }
00231
00232     BandedMatrixRow (BandedMatrix<T> &_m, int _row) : bm(_m), i(_row)
00233     { }
00234
00235     ~BandedMatrixRow () {}
00236
00237     typename BandedMatrix<T>::element_type & operator[] (int j)
00238     {
00239         return const_cast<BandedMatrix<T> &>(bm).element (i, j);
00240     }
00241
00242     const typename BandedMatrix<T>::element_type & operator[] (int j) const
00243     {
00244         return bm.element (i, j);
00245     }
00246
00247 private:
00248     const BandedMatrix<T> &bm;
00249     int i;
00250 };
00251
00252
00253 /*
00254  * Vector multiplication
00255  */
00256
00257 template <class Vector, class Matrix>
00258 Vector operator* (const Matrix &m, const Vector &v)
00259 {
00260     typename Matrix::size_type M = m.num_rows();
00261     typename Matrix::size_type N = m.num_cols();
00262
00263     assert (N <= v.size());
00264     //if (M > v.size())
00265     //  return Vector();
00266
00267     Vector r(N);
00268     for (unsigned int i = 0; i < M; ++i)
00269     {
00270         typename Matrix::element_type sum = 0;
00271         for (unsigned int j = 0; j < N; ++j)
00272         {
00273             sum += m[i][j] * v[j];
00274         }
00275         r[i] = sum;
00276     }
00277     return r;
00278 }
00279
00280
00281
00282 /*
00283  * LU factor a diagonally banded matrix using Crout's algorithm, but
00284  * limiting the trailing sub-matrix multiplication to the non-zero
00285  * elements in the diagonal bands.  Return nonzero if a problem occurs.
00286  */
00287 template <class MT>
00288 int LU_factor_banded (MT &A, unsigned int bands)
00289 {
00290     typename MT::size_type M = A.num_rows();
00291     typename MT::size_type N = A.num_cols();
00292     if (M != N)
00293         return 1;
00294
00295     typename MT::size_type i,j,k;
00296     typename MT::element_type sum;
00297
00298     for (j = 1; j <= N; ++j)
00299     {
00300         // Check for zero pivot
00301         if ( A(j,j) == 0 )
00302             return 1;
00303
00304         // Calculate rows above and on diagonal. A(1,j) remains as A(1,j).
00305         for (i = (j > bands) ? j-bands : 1; i <= j; ++i)
00306         {
00307             sum = 0;
00308             for (k = (j > bands) ? j-bands : 1; k < i; ++k)
00309             {
00310                 sum += A(i,k)*A(k,j);
00311             }
00312             A(i,j) -= sum;
00313         }
00314
00315         // Calculate rows below the diagonal.
00316         for (i = j+1; (i <= M) && (i <= j+bands); ++i)
00317         {
00318             sum = 0;
00319             for (k = (i > bands) ? i-bands : 1; k < j; ++k)
00320             {
00321                 sum += A(i,k)*A(k,j);
00322             }
00323             A(i,j) = (A(i,j) - sum) / A(j,j);
00324         }
00325     }
00326
00327     return 0;
00328 }
00329
00330
00331
00332 /*
00333  * Solving (LU)x = B.  First forward substitute to solve for y, Ly = B.
00334  * Then backwards substitute to find x, Ux = y.  Return nonzero if a
00335  * problem occurs.  Limit the substitution sums to the elements on the
00336  * bands above and below the diagonal.
00337  */
00338 template <class MT, class Vector>
00339 int LU_solve_banded(const MT &A, Vector &b, unsigned int bands)
00340 {
00341     typename MT::size_type i,j;
00342     typename MT::size_type M = A.num_rows();
00343     typename MT::size_type N = A.num_cols();
00344     typename MT::element_type sum;
00345
00346     if (M != N || M == 0)
00347         return 1;
00348
00349     // Forward substitution to find y.  The diagonals of the lower
00350     // triangular matrix are taken to be 1.
00351     for (i = 2; i <= M; ++i)
00352     {
00353         sum = b[i-1];
00354         for (j = (i > bands) ? i-bands : 1; j < i; ++j)
00355         {
00356             sum -= A(i,j)*b[j-1];
00357         }
00358         b[i-1] = sum;
00359     }
00360
00361     // Now for the backward substitution
00362     b[M-1] /= A(M,M);
00363     for (i = M-1; i >= 1; --i)
00364     {
00365         if (A(i,i) == 0)        // oops!
00366             return 1;
00367         sum = b[i-1];
00368         for (j = i+1; (j <= N) && (j <= i+bands); ++j)
00369         {
00370             sum -= A(i,j)*b[j-1];
00371         }
00372         b[i-1] = sum / A(i,i);
00373     }
00374
00375     return 0;
00376 }
00377
00378
00379 #endif /* _BANDEDMATRIX_ID */
00380
