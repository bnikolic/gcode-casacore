LatticeUtilities.h                                                                                  100644     715      12         5343  7005426530  13627  0                                                                                                    ustar   mholdawa                        wheel                                                                                                                                                                                                                  //# CopyLattice.h: global functions to copy a Lattices
//# Copyright (C) 1995,1996,1997,1999
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

#if !defined(AIPS_COPY_LATTICE_UTILITIES_H)
#define AIPS_LATTICE_UTILITIES_H


#include <aips/aips.h>

template <class T> class Lattice;
class IPosition;


// <summary>Global functions on Lattices</summary>
// <use visibility=export>

// <reviewed reviewer="" date="yyyy/mm/dd" tests="tCopyLattice.cc" demos="">
// </reviewed>
//
// <prerequisite>
//   <li> <linkto class="Lattice">Lattice</linkto>
// </prerequisite>
//
// <synopsis>
// Global functions using Lattices
// </synopsis>
//
// <example>
// <h4>Example 1:</h4>
// Copy the lattice-type data between two Images.// <srcblock>
//
// PagedImage<Float> myImg ("myimagefile");
// Float lmin;
// Float lmax;
// IPosition posMin = myImg.shape();
// IPosition posMax = myImg.shape();
// LatticeMinMax( lmin, lmax, posMin, posMax, myImg );
//
// </srcblock>
// </example>
//
//
// <motivation>
// Algorithms like CLEAN need to know the position of the MIN and MAX
// of an image, but easy things like LEL's min and max don't tell you
// the location of the min and max.  It seems there may be other global
// functions involving lattices.
// </motivation>
//
// <todo asof="1999/10/27">
//   <li> nothing I know of
// </todo>
//



// This global function finds the max of a Lattice, and also
// the IPositions of the max.  (LEL does not get you the IPositions of the
// min and max)
template <class T>
void LatticeMinMax(T & min, T & max, 
		   IPosition & posMin, IPosition & posMax, 
		   const Lattice<T>& lat);

#endif


                                                                                                                                                                                                                                                                                             test/tLatticeUtilities.cc                                                                           100644     715      12         5456  7005430014  15125  0                                                                                                    ustar   mholdawa                        wheel                                                                                                                                                                                                                  //# tLatticeUtilities.cc:
//# Copyright (C) 1997,1998,1999
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

#include <aips/aips.h>
#include <aips/Exceptions/Error.h>
#include <aips/Exceptions/Excp.h>
#include <aips/Lattices/IPosition.h>
#include <aips/Utilities/Assert.h>
#include <trial/Lattices/TempLattice.h>
#include <trial/Lattices/LatticeUtilities.h>

#include <iostream.h>


int main()
{
  try {

    TempLattice<Float> lat(IPosition(3,1024,1024,10), 1.0);
    lat.set(0.0);
    lat.putAt( 1.0, IPosition(3, 10, 10, 0) );
    lat.putAt( -1.0, IPosition(3, 20, 20, 2) );
    lat.putAt( 1.0, IPosition(3, 1000, 900, 3) );
    lat.putAt( -1.0, IPosition(3, 1002, 990, 4) );
    lat.putAt( 2.0, IPosition(3, 400, 500, 5) );
    lat.putAt( -2.0, IPosition(3, 10, 1000, 6) );
    lat.putAt( 3.0, IPosition(3, 900, 100, 7) );
    lat.putAt( -3.0, IPosition(3, 500, 100, 8) );
    lat.putAt( 4.0, IPosition(3, 1020, 1020, 8) );
    lat.putAt( -4.0, IPosition(3, 800, 800, 9) );

    Float lmin, lmax;
    IPosition lminPos(3, 0);
    IPosition lmaxPos(3, 0);

    LatticeMinMax(lmin, lmax, lminPos, lmaxPos, lat);

    IPosition trueMaxPos = IPosition(3, 1020, 1020, 8);
    IPosition trueMinPos = IPosition(3, 800, 800, 9);
    if (trueMaxPos == lmaxPos && lmax == 4.0) {
      cout << "Max find in LatticeMinMax succeeded" << endl;
    }
    if (trueMinPos == lminPos && lmin == -4.0) {
      cout << "Min find in LatticeMinMax succeeded" << endl;
    }

  } catch (AipsError x) {
    cout<< "FAIL"<< endl;
    cerr << x.getMesg() << endl;
    return 1;
  } end_try;
  cout<< "OK"<< endl;
  return 0;
};

// Local Variables: 
// compile-command: "gmake OPTLIB=1 tLatticeUtilities"
// End: 
                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                  