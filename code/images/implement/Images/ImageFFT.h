//# ImageFFT.h: FFT an image
//# Copyright (C) 1996,1997,1998,1999
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

#if !defined(AIPS_IMAGEFFT_H)
#define AIPS_IMAGEFFT_H

#include <aips/aips.h>
#include <trial/Images/ImageInterface.h>
#include <trial/Coordinates/Coordinate.h>

class CoordinateSystem;
class IPosition;
template<class T> class Vector;

// <summary>
// FFT an image
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=LatticeFFT>LatticeFFT</linkto> 
//   <li> <linkto class=ImageInterface>ImageInterface</linkto> 
//   <li> <linkto class=TempImage>TempImage</linkto> 
// </prerequisite>

// <etymology>
// Take the fast Fourier Transform of an image.
// </etymology>

// <synopsis>
// This class takes the FFT of an image.  It can
// take the FFT of just the sky plane(s) of an image or
// the specified axes.
//
// When you specify axes, if any of them are a sky axis (DirectionCoordinate)
// you must give both sky axes.
//
// Masked pixels are given the value 0.0 before the FFT is taken and the
// mask is copied to the output.  Note that it is the callers responsibility
// to give the output a mask if the input is masked.  Otherwise the mask
// will not be copied to the output
//
// This class holds the FourierTransform internally in a 
// <linkto class=TempImage>TempImage</linkto> object.  This is
// in memory or on disk depending upon its size and the amount
// of memory in your computer.    The algorithm used
// is that in <linkto class=TempLattice>TempLattice</linkto>.
//
// In generating the Fourier Coordinates, it is currently
// assumed that there is no coordinate rotation.  This 
// needs to be dealt with.
// </synopsis>
//
// <example>
// <srcblock>
//
//// Make a constant image
//
//      IPosition shape(2, 10, 20);
//      PagedImage<Float> im1(shape, CoordinateUtil::defaultCoords2D(), "im1");
//      im1.set(1.0);
//
//// Create output images with masks if needed
//
//      PagedImage<Float> r1(shape, CoordinateUtil::defaultCoords2D(), "real1");
//      PagedImage<Float> i1(shape, CoordinateUtil::defaultCoords2D(), "imag1");
//      if (im1.isMasked()) {
//         LCPagedMask mask1 = LCPagedMask(RegionHandler::makeMask (r1, "mask0"));
//         r1.defineRegion ("mask0", ImageRegion(mask1), RegionHandler::Masks);
//         r1.setDefaultMask("mask0");
//         LCPagedMask mask2 = LCPagedMask(RegionHandler::makeMask (i1, "mask0"));
//         i1.defineRegion ("mask0", ImageRegion(mask1), RegionHandler::Masks);
//         i1.setDefaultMask("mask0");
//      }
///
/// FFT
//
//      ImageFFT fft;
//      fft.fftsky(im1);
//
//// The coordinates and mask will be overwritten
//
//      fft.getReal(r1);
//      fft.getImag(i1);
// </srcblock>
// </example>


// <motivation>
// Taking the Fourier Transform of an image is a basic part of image analysis
// </motivation>

// <todo asof="1999/09/23">
//   <li> FT axes other than the sky axes
//   <li> reverse transformations
// </todo>


class ImageFFT 
{
public:

// Default constructor
   ImageFFT ();

// Copy constructor (reference semantics)
   ImageFFT(const ImageFFT& other);

// Assignment (reference semantics)
   ImageFFT& operator=(const ImageFFT& other);

// Destructor
   ~ImageFFT();

// Do the FFT of the sky plane to the uv plane
   void fftsky (const ImageInterface<Float>& in);

// Do the FFT of the specified pixel axes (True to FT).  
// The rest are iterated over
   void fft (const ImageInterface<Float>& in, 
             const Vector<Bool>& axes);

// Return the FFT (from the last call to fftsky or fft) in the 
// desired form.    The CoordinateSystem of
// the image is overwritten by these functions to reflect the 
// Fourier axes.  
// <group>
   void getComplex (ImageInterface<Complex>& out) const;
   void getReal (ImageInterface<Float>& out) const;
   void getImaginary (ImageInterface<Float>& out) const;
   void getAmplitude (ImageInterface<Float>& out) const;
   void getPhase (ImageInterface<Float>& out) const;
// </group>

// This returns a reference to the internal complex 
// FFT.  It is only useful for getting at the pixel
// values as it has no mask.  Its coordinates are correct.
   const ImageInterface<Complex>& getComplex () const;

private:

   ImageInterface<Complex>* itsTempImagePtr;
   ImageInterface<Float>* itsInImagePtr;
   Bool itsDone;
   Coordinate* itsCoordPtr;

// Check axes for multi-dim FFT
   void checkAxes(const ImageInterface<Float>& in, const Vector<Bool>& axes);

// Copy the  mask to the output
   void copyMask (ImageInterface<Float>& out,
                  const ImageInterface<Float>& in) const;
   void copyMask (ImageInterface<Complex>& out,
                  const ImageInterface<Float>& in) const;

// Copy some bits and pieces from the input to the output
// images
   void copyMiscellaneous (ImageInterface<Float>& out,   
                           const ImageInterface<Float>& in) const;
   void copyMiscellaneous (ImageInterface<Complex>& out,   
                           const ImageInterface<Float>& in) const;

// FFT the sky
   void fftsky2 (ImageInterface<Complex>& out,
                 const ImageInterface<Float>& in,
                 const Vector<Int>& pixelAxes);

// FFT given axes
   void fft2(ImageInterface<Complex>& out,
             const ImageInterface<Float>& in,
             const Vector<Bool>& axes);

// Find the sky axes in this CoordinateSystem
   Bool findSky(LogIO& os, Int& dC, Vector<Int>& pixelAxes,
                Vector<Int>& worldAxes, const CoordinateSystem& cSys,
                Bool throwIt);

// Overwrite the coordinates for the sky axes
// with Fourier coordinates
   void setCoordinates (LogIO& os,
                        ImageInterface<Complex>& out,
                        const ImageInterface<Float>& in,
                        const Vector<Int>& pixelAxes,
                        const Vector<Int>& worldAxes,
                        const Int dC);

// Overwrite the coordinates for the FT axes
   void setCoordinates2 (LogIO& os,
                         ImageInterface<Complex>& out,
                         const ImageInterface<Float>& in,
                         const Vector<Bool>& axes);


// Set Fourier coordinate
   void setInverseCoordinate (CoordinateSystem& cSys,
                              uInt which, const IPosition& shape,
                              const Vector<Bool>& axes);
};
#endif
