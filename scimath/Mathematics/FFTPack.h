//# FFTPack.h: C++ wrapper functions for Fortran FFTPACK code
//# Copyright (C) 1993,1994,1995,1997,1999,2000,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#

//# $Id$

#ifndef SCIMATH_FFTPACK_H
#define SCIMATH_FFTPACK_H

#include <casacore/casa/aips.h>

  //# The SGI compiler with -LANG:std has some trouble including both Complexfwd.h
  //# and Complex.h so we bypass the problem by include Complex.h only.
#if defined(AIPS_USE_NEW_SGI)
#include <casa/BasicSL/Complex.h>
#else
#include <casa/BasicSL/Complexfwd.h>
#endif

namespace casa { //# NAMESPACE CASA - BEGIN

// <summary>C++ interface to the Fortran FFTPACK library</summary>
// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="" demos="">
// </reviewed>
// <synopsis>
// The static functions in this class are C++ wrappers to the Fortran FFTPACK
// library. This library contains functions that perform fast Fourier
// transforms (FFT's) and related transforms. 

// An additional purpose of these definitions is to overload the functions so
// that C++ users can access the functions in either fftpak (single precision)
// or dfftpack (double precision) with identical function names.

// These routines only do one-dimensional transforms with the first element of
// the array being the "origin" of the transform. The <linkto
// class="FFTServer">FFTServer</linkto> class uses some of these functions to
// implement multi-dimensional transforms with the origin of the transform
// either at the centre or the first element of the Array.

// You must initialise the work array <src>wsave</src> before using the forward
// transform (function with a suffix of f) or the backward transform (with a
// suffix of b).

// The transforms done by the functions in this class can be categorised as
// follows:
// <ul>
// <li> Complex to Complex Transforms<br> 
//      Done by the cttfi, cfftf & cfftb functions
// <li> Real to Complex Transforms<br>
//      Done by the rffti, rfftf & rfftb functions. A simpler interface is
//      provided by the ezffti, ezfftf & ezfftb functions. The 'ez' functions
//      do not destroy the input array and provide the result in a slightly
//      less packed format. They are available in single precision only and
//      internally use the rfft functions.
// <li> Sine Transforms<br>
//      Done by the sinti & sint functions. As the sine transform is its own
//      inverse there is no need for any distinction between forward and
//      backward transforms.
// <li> Cosine Transforms<br>
//      Done by the costi & cost functions. As the cosine transform is its own
//      inverse there is no need for any distinction between forward and
//      backward transforms.
// <li> Sine quarter wave Transforms<br>
//      Done by the sinqi, sinqf & sinqb functions.
// <li> Cosine quarter wave Transforms<br>
//      Done by the cosqi, cosqf & cosqb functions.
// </ul>


// <note role=warning> 
// These functions assume that it is possible to convert between Casacore numeric
// types and those used by Fortran. That it is possible to convert between
// Float & float, Double & double and Int & int.
// </note>

// <note role=warning> 
// These function also assume that a Complex array is stored as pairs of
// floating point numbers, with no intervening gaps, and with the real
// component first ie., <src>[re0,im0,re1,im1, ...]</src> so that the following
// type casts work,
// <srcblock>
// Complex* complexPtr;
// Float* floatPtr = (Float* ) complexPtr;
// </srcblock>
// and allow a Complex number to be accessed as a pair of real numbers. If this
// assumption is bad then float Arrays will have to generated by copying the
// complex ones. When compiled in debug mode mode the functions that require
// this assumption will throw an exception (AipsError) if this assumption is
// bad. Ultimately this assumption about Complex<->Float Array conversion
// should be put somewhere central like Array2Math.cc.
// </note>

// </synopsis>


class FFTPack
{
public:
// cffti initializes the array wsave which is used in both <src>cfftf</src> and
// <src>cfftb</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 4*n+15
//         The same work array can be used for both cfftf and cfftb
//         as long as n remains unchanged. Different wsave arrays
//         are required for different values of n. The contents of
//         wsave must not be changed between calls of cfftf or cfftb.
// </dl>
// <group>
static void cffti(Int n, Float* wsave);
static void cffti(Int n, Double* wsave);
//Here is the doc from FFTPack 5.1 
//You can convert the linguo from fortran to C/C++
/*  Input Arguments
 
 L       Integer number of elements to be transformed in the first 
         dimension.  The transform is most efficient when L is a 
         product of small primes.


 M       Integer number of elements to be transformed in the second 
         dimension.  The transform is most efficient when M is a 
         product of small primes.
 
 LENSAV  Integer dimension of WSAVE array.  LENSAV must be at least 
         2*(L+M) + INT(LOG(REAL(L))/LOG(2.)) + INT(LOG(REAL(M))/LOG(2.)) + 8.


 Output Arguments
 
 WSAVE   Real work array with dimension LENSAV, containing the
         prime factors of L and M, and also containing certain 
         trigonometric values which will be used in routines 
         CFFT2B or CFFT2F.


 WSAVE   Real work array with dimension LENSAV.  The WSAVE array 
         must be initialized with a call to subroutine CFFT2I before 
         the first call to CFFT2B or CFFT2F, and thereafter whenever
         the values of L, M or the contents of array WSAVE change.  
         Using different WSAVE arrays for different transform lengths
         or types in the same program may reduce computation costs 
         because the array contents can be re-used.


 IER     Integer error return
         =  0 successful exit
         =  2 input parameter LENSAV not big enough
         = 20 input error returned by lower level routine
*/

static void cfft2i(const Int& n, const Int& m, Float *& wsave, const Int& lensav, Int& ier);
// </group>

// cfftf computes the forward complex discrete Fourier
// transform (the Fourier analysis). Equivalently, cfftf computes
// the Fourier coefficients of a complex periodic sequence.
// the transform is defined below at output parameter c.
// 
// The transform is not normalized. To obtain a normalized transform
// the output must be divided by n. Otherwise a call of cfftf
// followed by a call of cfftb will multiply the sequence by n.
// 
// The array wsave which is used by <src>cfftf</src> must be
// initialized by calling <src>cffti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>   The length of the complex sequence c. The method is
//        more efficient when n is the product of small primes.
// <dt><b>c</b>
// <dd>    A complex array of length n which contains the sequence to be
//         transformed.
// <dt><b>wsave</b>
// <dd>    A real work array which must be dimensioned at least 4n+15
//         by the program that calls cfftf. The wsave array must be
//         initialized by calling <src>cffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by cfftf and cfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>c</b>
// <dd>   for j=1,...,n<br>
//            c(j)=the sum from k=1,...,n of<br>
//                  c(k)*exp(-i*(j-1)*(k-1)*2*pi/n)<br>
//                        where i=sqrt(-1)<br>
// <dt><b>wsave</b>
// <dd>    Contains initialization calculations which must not be
//         destroyed between calls of cfftf or cfftb
// </dl>
// <group>
static void cfftf(Int n, Complex* c, Float* wsave);
static void cfftf(Int n, DComplex* c, Double* wsave);

//Description from FFTPack 5.1
/*
Input Arguments


 LDIM    Integer first dimension of two-dimensional complex array C.


 L       Integer number of elements to be transformed in the first 
         dimension of the two-dimensional complex array C.  The value 
         of L must be less than or equal to that of LDIM.  The 
         transform is most efficient when L is a product of small 
         primes.


 M       Integer number of elements to be transformed in the second 
         dimension of the two-dimensional complex array C.  The 
         transform is most efficient when M is a product of small 
         primes.
 
 C       Complex array of two dimensions containing the (L,M) subarray
         to be transformed.  C's first dimension is LDIM, its second 
         dimension must be at least M.

 WSAVE   Real work array with dimension LENSAV.  WSAVE's contents 
         must be initialized with a call to subroutine CFFT2I before 
         the first call to routine CFFT2F or CFFT2B with transform 
         lengths L and M.  WSAVE's contents may be re-used for 
         subsequent calls to CFFT2F and CFFT2B having those same 
         transform lengths.
 


 LENSAV  Integer dimension of WSAVE array.  LENSAV must be at least 
         2*(L+M) + INT(LOG(REAL(L))/LOG(2.)) + INT(LOG(REAL(M))/LOG(2.)) + 8.


 WORK    Real work array.


 LENWRK  Integer dimension of WORK array.  LENWRK must be at least
         2*L*M.
*/
static void cfft2f (const Int& ldim, const Int& L, const Int& M, Complex*& C, Float*& WSAVE, const Int& LENSAV,
		    Float *& WORK, const Int& LENWRK, Int& IER);
// </group>

// cfftb computes the backward complex discrete Fourier
// transform (the Fourier synthesis). Equivalently, cfftb computes
// a complex periodic sequence from its Fourier coefficients.
// The transform is defined below with output parameter c.
// 
// A call of cfftf followed by a call of cfftb will multiply the
// sequence by n.
// 
// The array wsave which is used by <src>cfftb</src> must be
// initialized by calling <src>cffti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>          The length of the complex sequence c. The method is
//               more efficient when n is the product of small primes.
// <dt><b>c</b>
// <dd>          A complex array of length n which contains the sequence to be
//               transformed.
// <dt><b>wsave</b> 
// <dd>          A real work array which must be dimensioned at least 4n+15
//               in the program that calls cfftb. The wsave array must be
//               initialized by calling <src>cffti(n,wsave)</src>
//               and a different wsave array must be used for each different
//               value of n. This initialization does not have to be
//               repeated so long as n remains unchanged thus subsequent
//               transforms can be obtained faster than the first.
//               The same wsave array can be used by cfftf and cfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>c</b>     
// <dd>          for j=1,...,n<br>
//                 c(j)=the sum from k=1,...,n of<br>
//                     c(k)*exp(i*(j-1)*(k-1)*2*pi/n)<br>

// <dt><b>wsave</b>
// <dd>          Contains initialization calculations which must not be
//               destroyed between calls of cfftf or cfftb
// </dl>
// <group>
static void cfftb(Int n, Complex* c, Float* wsave);
static void cfftb(Int n, DComplex* c, Double* wsave);


//Documentation from FFTPack 5.1
/*
 Input Arguments
 
 LDIM    Integer first dimension of two-dimensional complex array C.
 

 L       Integer number of elements to be transformed in the first 
         dimension of the two-dimensional complex array C.  The value 
         of L must be less than or equal to that of LDIM.  The 
         transform is most efficient when L is a product of small 
         primes.


 M       Integer number of elements to be transformed in the second 
         dimension of the two-dimensional complex array C.  The 
         transform is most efficient when M is a product of small 
         primes.
 
 C       Complex array of two dimensions containing the (L,M) subarray
         to be transformed.  C's first dimension is LDIM, its second 
         dimension must be at least M.


 WSAVE   Real work array with dimension LENSAV.  WSAVE's contents 
         must be initialized with a call to subroutine CFFT2I before 
         the first call to routine CFFT2F or CFFT2B with transform 
         lengths L and M.  WSAVE's contents may be re-used for 
         subsequent calls to CFFT2F and CFFT2B with the same 
         transform lengths L and M.

 LENSAV  Integer dimension of WSAVE array.  LENSAV must be at least 
         2*(L+M) + INT(LOG(REAL(L))/LOG(2.)) + INT(LOG(REAL(M))/LOG(2.)) + 8.

 WORK    Real work array.
 
 LENWRK  Integer dimension of WORK array.  LENWRK must be at least
         2*L*M.
 
 Output Arguments


  C      Complex output array.  For purposes of exposition,
         assume the index ranges of array C are defined by
         C(0:L-1,0:M-1).
 
         For I=0,...,L-1 and J=0,...,M-1, the C(I,J)'s are given
          in the traditional aliased form by
 
                   L-1  M-1
          C(I,J) = SUM  SUM  C(L1,M1)*
                   L1=0 M1=0
 
                     EXP(SQRT(-1)*2*PI*(I*L1/L + J*M1/M))
 
          And in unaliased form, the C(I,J)'s are given by
 
                   LF    MF
          C(I,J) = SUM   SUM   C(L1,M1,K1)*
                   L1=LS M1=MS
 
                     EXP(SQRT(-1)*2*PI*(I*L1/L + J*M1/M))
 
          where
 
             LS= -L/2    and LF=L/2-1   if L is even;
             LS=-(L-1)/2 and LF=(L-1)/2 if L is odd;
             MS= -M/2    and MF=M/2-1   if M is even;
             MS=-(M-1)/2 and MF=(M-1)/2 if M is odd;
 
          and
 
             C(L1,M1) = C(L1+L,M1) if L1 is zero or negative;
             C(L1,M1) = C(L1,M1+M) if M1 is zero or negative;
 
          The two forms give different results when used to
          interpolate between elements of the sequence.
 
  IER     Integer error return
          =  0 successful exit
          =  2 input parameter LENSAV not big enough
          =  3 input parameter LENWRK not big enough
          =  5 input parameter L > LDIM
          = 20 input error returned by lower level routine
*/



static void cfft2b (const Int& LDIM, const Int& L, const Int& M, Complex* & C, Float *& WSAVE, const Int& LENSAV,
		    Float*& WORK, const Int& LENWRK, Int& IER);
// </group>

// rffti initializes the array wsave which is used in both <src>rfftf</src> and
// <src>rfftb</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
//
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>       The length of the sequence to be transformed.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 2*n+15.
//         The same work array can be used for both rfftf and rfftb
//         as long as n remains unchanged. Different wsave arrays
//         are required for different values of n. The contents of
//         wsave must not be changed between calls of rfftf or rfftb.
// </dl>
// <group>
static void rffti(Int n, Float* wsave);
static void rffti(Int n, Double* wsave);
// </group>

// rfftf computes the Fourier coefficients of a real perodic sequence (Fourier
// analysis). The transform is defined below at output parameter r.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the array r to be transformed.  The method
//         is most efficient when n is a product of small primes.
//         n may change so long as different work arrays are provided
// <dt><b>r</b>
// <dd>    A real array of length n which contains the sequence
//         to be transformed
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 2*n+15
//         in the program that calls rfftf. The wsave array must be
//         initialized by calling <src>rffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by rfftf and rfftb.
// </dl>
// output parameters
// <dl compact>
// <dt><b>r</b>
// <dd>    r(1) = the sum from i=1 to i=n of r(i)<br>
//         if n is even set l = n/2   , if n is odd set l = (n+1)/2<br>
//         then for k = 2,...,l<br>
//              r(2*k-2) = the sum from i = 1 to i = n of<br>
//                         r(i)*cos((k-1)*(i-1)*2*pi/n)<br>
//              r(2*k-1) = the sum from i = 1 to i = n of<br>
//                         -r(i)*sin((k-1)*(i-1)*2*pi/n)<br>
//         if n is even<br>
//              r(n) = the sum from i = 1 to i = n of<br>
//                   (-1)**(i-1)*r(i)<br>
// 
//         note:
//              this transform is unnormalized since a call of rfftf
//              followed by a call of rfftb will multiply the input
//              sequence by n.
// <dt><b>wsave</b>
// <dd>    Contains results which must not be destroyed between
//         calls of rfftf or rfftb.
// </dl>
// <group>
static void rfftf(Int n, Float* r, Float* wsave);
static void rfftf(Int n, Double* r, Double* wsave);
// </group>


// rfftb computes the real perodic sequence from its Fourier coefficients
// (Fourier synthesis). The transform is defined below at output parameter r.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the array r to be transformed.  The method
//         is most efficient when n is a product of small primes.
//         n may change so long as different work arrays are provided
// <dt><b>r</b>
// <dd>    A real array of length n which contains the sequence
//         to be transformed
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 2*n+15
//         in the program that calls rfftb. The wsave array must be
//         initialized by calling <src>rffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by rfftf and rfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>r</b>
// <dd>    for n even and for i = 1,...,n<br>
//              r(i) = r(1)+(-1)**(i-1)*r(n)<br>
//                   plus the sum from k=2 to k=n/2 of<br>
//                    2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)<br>
//                   -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)<br>
//         for n odd and for i = 1,...,n<br>
//              r(i) = r(1) plus the sum from k=2 to k=(n+1)/2 of<br>
//                   2.*r(2*k-2)*cos((k-1)*(i-1)*2*pi/n)<br>
//                  -2.*r(2*k-1)*sin((k-1)*(i-1)*2*pi/n)<br>
// 
//         note:
//              this transform is unnormalized since a call of rfftf
//              followed by a call of rfftb will multiply the input
//              sequence by n.
// <dt><b>wsave</b>
// <dd>    Contains results which must not be destroyed between
//         calls of rfftb or rfftf.
// </dl>
// <group>
static void rfftb(Int n, Float* r, Float* wsave);
static void rfftb(Int n, Double* r, Double* wsave);
// </group>

// ezffti initializes the array wsave which is used in both <src>ezfftf</src>
// and <src>ezfftb</src>. The prime factorization of n together with a
// tabulation of the trigonometric functions are computed and stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15.
//         The same work array can be used for both ezfftf and ezfftb
//         as long as n remains unchanged. Different wsave arrays
//         are required for different values of n.
// </dl>
static void ezffti(Int n, Float* wsave);

// ezfftf computes the Fourier coefficients of a real
// perodic sequence (Fourier analysis). The transform is defined
// below at output parameters azero, a and b. ezfftf is a simplified
// but slower version of rfftf.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the array r to be transformed.  The method
//         is most efficient when n is the product of small primes.
// <dt><b>r</b>
// <dd>    A real array of length n which contains the sequence
//         to be transformed. r is not destroyed.
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15
//         in the program that calls ezfftf. The wsave array must be
//         initialized by calling <src>ezffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by ezfftf and ezfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>azero</b>
// <dd>    The sum from i=1 to i=n of r(i)/n
// <dt><b>a,b</b>
// <dd>    Real arrays of length n/2 (n even) or (n-1)/2 (n odd)<br>
//         for n even<br>
//            b(n/2)=0, and <br>
//            a(n/2) is the sum from i=1 to i=n of (-1)**(i-1)*r(i)/n<br>
// 
//         for n even define kmax=n/2-1<br>
//         for n odd  define kmax=(n-1)/2<br>
//         then for  k=1,...,kmax<br>
//            a(k) equals the sum from i=1 to i=n of<br>
//                   2./n*r(i)*cos(k*(i-1)*2*pi/n)<br>
//            b(k) equals the sum from i=1 to i=n of<br>
//                   2./n*r(i)*sin(k*(i-1)*2*pi/n)<br>
// </dl>
static void ezfftf(Int n, Float* r, Float* azero, Float* a, Float* b, 
	    Float* wsave);

// ezfftb computes a real perodic sequence from its
// Fourier coefficients (Fourier synthesis). The transform is
// defined below at output parameter r. ezfftb is a simplified
// but slower version of rfftb.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>       
// <dd>    The length of the output array r.  The method is most
//         efficient when n is the product of small primes.
// <dt><b>azero</b>
// <dd>    The constant Fourier coefficient
// <dt><b>a,b</b>
// <dd>    Arrays which contain the remaining Fourier coefficients
//         these arrays are not destroyed.
//         The length of these arrays depends on whether n is even or
//         odd.
//         If n is even n/2    locations are required,
//         if n is odd (n-1)/2 locations are required.
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15.
//         in the program that calls ezfftb. The wsave array must be
//         initialized by calling <src>ezffti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
//         The same wsave array can be used by ezfftf and ezfftb.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>r</b>
// <dd>    if n is even define kmax=n/2<br>
//         if n is odd  define kmax=(n-1)/2<br>
//         then for i=1,...,n<br>
//              r(i)=azero plus the sum from k=1 to k=kmax of<br>
//              a(k)*cos(k*(i-1)*2*pi/n)+b(k)*sin(k*(i-1)*2*pi/n)<br>
//         where<br>
//              c(k) = .5*cmplx(a(k),-b(k))   for k=1,...,kmax<br>
//              c(-k) = conjg(c(k))<br>
//              c(0) = azero<br>
//                   and i=sqrt(-1)<br>
// </dl>
static void ezfftb(Int n, Float* r, Float* azero, Float* a, Float* b, 
	    Float* wsave);

// sinti initializes the array wsave which is used in
// <src>sint</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed.  the method
//         is most efficient when n+1 is a product of small primes.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array with at least int(2.5*n+15) locations.
//         Different wsave arrays are required for different values
//         of n. The contents of wsave must not be changed between
//         calls of sint.
// </dl>
// <group>
static void sinti(Int n, Float* wsave);
static void sinti(Int n, Double* wsave);
// </group>

// sint computes the discrete Fourier sine transform
// of an odd sequence x(i). The transform is defined below at
// output parameter x.
// sint is the unnormalized inverse of itself since a call of sint
// followed by another call of sint will multiply the input sequence
// x by 2*(n+1).
// The array wsave which is used by sint must be
// initialized by calling <src>sinti(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed.  The method
//         is most efficient when n+1 is the product of small primes.
// <dt><b>x</b>
// <dd>    An array which contains the sequence to be transformed
// <dt><b>wsave</b>
// <dd>    A work array with dimension at least int(2.5*n+15)
//         in the program that calls sint. The wsave array must be
//         initialized by calling <src>sinti(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//              x(i) = the sum from k=1 to k=n<br>
//                   2*x(k)*sin(k*i*pi/(n+1))<br>
// 
//              a call of sint followed by another call of
//              sint will multiply the sequence x by 2*(n+1).
//              Hence sint is the unnormalized inverse
//              of itself.
// 
// <dt><b>wsave</b>
// <dd>    Contains initialization calculations which must not be
//         destroyed between calls of sint.
// </dl>
// <group>
static void sint(Int n, Float* x, Float* wsave);
static void sint(Int n, Double* x, Double* wsave);
// </group>

// costi initializes the array wsave which is used in
// <src>cost</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
// 
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed.  The method
//         is most efficient when n-1 is a product of small primes.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15.
//         Different wsave arrays are required for different values
//         of n. The contents of wsave must not be changed between
//         calls of cost.
// </dl>
// <group>
static void costi(Int n, Float* wsave);
static void costi(Int n, Double* wsave);
// </group>

// cost computes the discrete Fourier cosine transform
// of an even sequence x(i). The transform is defined below at output
// parameter x.
// cost is the unnormalized inverse of itself since a call of cost
// followed by another call of cost will multiply the input sequence
// x by 2*(n-1). The transform is defined below at output parameter x.
// The array wsave which is used by <src>cost</src> must be
// initialized by calling <src>costi(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence x. n must be greater than 1.
//         The method is most efficient when n-1 is a product of
//         small primes.
// <dt><b>x</b>
// <dd>    An array which contains the sequence to be transformed
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15
//         in the program that calls cost. The wsave array must be
//         initialized by calling <src>costi(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//             x(i) = x(1)+(-1)**(i-1)*x(n)<br>
//              + the sum from k=2 to k=n-1<br>
//                  2*x(k)*cos((k-1)*(i-1)*pi/(n-1))<br>
// 
//              a call of cost followed by another call of
//              cost will multiply the sequence x by 2*(n-1)
//              hence cost is the unnormalized inverse
//              of itself.
// <dt><b>wsave</b>
// <dd>    Contains initialization calculations which must not be
//         destroyed between calls of cost.
// </dl>
// <group>
static void cost(Int n, Float* x, Float* wsave);
static void cost(Int n, Double* x, Double* wsave);
// </group>

// sinqi initializes the array wsave which is used in both <src>sinqf</src> and
// <src>sinqb</src>. The prime factorization of n together with a tabulation of
// the trigonometric functions are computed and stored in wsave.
//
// Input parameter:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the sequence to be transformed. The method
//         is most efficient when n is a product of small primes.
// </dl>
// Output parameter:
// <dl compact>
// <dt><b>wsave</b>
// <dd>    A work array which must be dimensioned at least 3*n+15.
//         The same work array can be used for both sinqf and sinqb
//         as long as n remains unchanged. Different wsave arrays
//         are required for different values of n. The contents of
//         wsave must not be changed between calls of sinqf or sinqb.
// </dl>
// <group>
static void sinqi(Int n, Float* wsave);
static void sinqi(Int n, Double* wsave);
// </group>

// sinqf computes the fast Fourier transform of quarter wave data. That is,
// sinqf computes the coefficients in a sine series representation with only
// odd wave numbers. The transform is defined below at output parameter x.
// 
// sinqb is the unnormalized inverse of sinqf since a call of sinqf followed by
// a call of sinqb will multiply the input sequence x by 4*n.
// 
// The array wsave which is used by sinqf must be initialized by calling
// <src>sinqi(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the array x to be transformed.  The method
//         is most efficient when n is a product of small primes.
// <dt><b>x</b>
// <dd>    An array which contains the sequence to be transformed
// <dt><b>wsave</b>
//         A work array which must be dimensioned at least 3*n+15.
//         in the program that calls sinqf. The wsave array must be
//         initialized by calling <src>sinqi(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//              x(i) = (-1)**(i-1)*x(n)<br>
//                 + the sum from k=1 to k=n-1 of<br>
//                 2*x(k)*sin((2*i-1)*k*pi/(2*n))<br>
// 
//              a call of sinqf followed by a call of
//              sinqb will multiply the sequence x by 4*n.
//              therefore sinqb is the unnormalized inverse
//              of sinqf.
// <dt><b>wsave </b>
// <dd>    Contains initialization calculations which must not
//         be destroyed between calls of sinqf or sinqb.
// </dl>
// <group>
static void sinqf(Int n, Float* x, Float* wsave);
static void sinqf(Int n, Double* x, Double* wsave);
// </group>

// sinqb computes the fast Fourier transform of quarter
// wave data. that is, sinqb computes a sequence from its
// representation in terms of a sine series with odd wave numbers.
// the transform is defined below at output parameter x.
// 
// sinqf is the unnormalized inverse of sinqb since a call of sinqb
// followed by a call of sinqf will multiply the input sequence x
// by 4*n.
// 
// The array wsave which is used by <src>sinqb</src> must be
// initialized by calling <src>sinqi(n,wsave)</src>.
// 
// Input parameters:
// <dl compact>
// <dt><b>n</b>
// <dd>    The length of the array x to be transformed.  The method
//         is most efficient when n is a product of small primes.
// <dt><b>x</b>
// <dd>    An array which contains the sequence to be transformed
// <dt><b>wsave</b>
//         A work array which must be dimensioned at least 3*n+15.
//         in the program that calls sinqb. The wsave array must be
//         initialized by calling <src>sinqi(n,wsave)</src> and a
//         different wsave array must be used for each different
//         value of n. This initialization does not have to be
//         repeated so long as n remains unchanged thus subsequent
//         transforms can be obtained faster than the first.
// </dl>
// Output parameters:
// <dl compact>
// <dt><b>x</b>
// <dd>    for i=1,...,n<br>
//              x(i)= the sum from k=1 to k=n of<br>
//                4*x(k)*sin((2k-1)*i*pi/(2*n))<br>
// 
//              a call of sinqb followed by a call of
//              sinqf will multiply the sequence x by 4*n.
//              Therefore sinqf is the unnormalized inverse
//              of sinqb.
// <dt><b>wsave</b>
// <dd>    Contains initialization calculations which must not
//         be destroyed between calls of sinqb or sinqf.
// </dl>
// <group>
static void sinqb(Int n, Float* x, Float* wsave);
static void sinqb(Int n, Double* x, Double* wsave);
// </group>

// <group>
static void cosqi(Int n, Float* wsave);
static void cosqi(Int n, Double* wsave);
// </group>
// <group>
static void cosqf(Int n, Float* x, Float* wsave);
static void cosqf(Int n, Double* x, Double* wsave);
// </group>
// <group>
static void cosqb(Int n, Float* x, Float* wsave);
static void cosqb(Int n, Double* x, Double* wsave);
// </group>
};

} //# NAMESPACE CASA - END

#endif
