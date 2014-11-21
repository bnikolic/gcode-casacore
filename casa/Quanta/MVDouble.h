//# MVDouble.h: class to distinguish between internal and external Double
//# Copyright (C) 1996,1997,1998,1999,2000,2001
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

#ifndef CASA_MVDOUBLE_H
#define CASA_MVDOUBLE_H


//# Includes
#include <casacore/casa/aips.h>
#include <casacore/casa/Arrays/Vector.h>
#include <casacore/casa/Quanta/Quantum.h>
#include <casacore/casa/Quanta/MeasValue.h>
#include <casacore/casa/iosfwd.h>

namespace casa { //# NAMESPACE CASA - BEGIN

//# Forward Declarations

//# Constants (SUN compiler does not accept non-simple default arguments)

// <summary> Class to distinguish external and Measure internal Double </summary>

// <use visibility=export>

// <reviewed reviewer="tcornwel" date="1996/02/23" tests="tMeasMath" demos="">
// </reviewed>

// <prerequisite>
// <li> <linkto class=MeasValue>MeasValue</linkto>
// </prerequisite>
//
// <etymology>
// From Measure, Value and Double
// </etymology>
//
// <synopsis>
// An MVDouble is a simple Double, to be used in simple, single value
// Measures. Requirements can be found in the 
// <linkto class=MeasValue>MeasValue</linkto> base class.<br>
// The only reasonable constructor is (but all MeasValue constructors are present)
// <src>MVDouble(Double)</src>; and an <src>operator Double</src> takes
// care of all other possibilities. Its external use is for
//  <linkto class=MeasConvert>MeasConvert</linkto>, to distinguish between
// input in internal Measure units, and values which have to have
// units applied.
// </synopsis>
//
// <example>
// See e.g. <linkto class=MFrequency>MFrequency</linkto>
// </example>
//
// <motivation>
// To aid coordinate transformations possibilities
// </motivation>
//
// <todo asof="1995/09/04">
// </todo>

class MVDouble : public MeasValue {

public:
  
  //# Constructors
  // Default constructor: generate a zero value
  MVDouble();
  // Copy constructor
  MVDouble(const MVDouble &other);
  // Copy assignment
  MVDouble &operator=(const MVDouble &other);
  // Constructor from Double
  MVDouble(Double d);
  // Constructor from Quantum : value taken will be the canonical value
  // <group>
  MVDouble(const Quantity &other);
  MVDouble(const Quantum<Vector<Double> > &other);
  // </group>
  // Constructor from Vector. A zero value will be taken for an empty vector,
  // the canonical value for a quantum vector.
  // <thrown>
  //  <li> AipsError if vector length > 1
  // </thrown>
  // <group>
  MVDouble(const Vector<Double> &other);
  MVDouble(const Vector<Quantity> &other);
  // </group>
  
  // Destructor
  ~MVDouble();
  
  //# Operators
  // Conversion operator
  operator Double() const;
  
  // Addition
  // <group>
  MVDouble &operator+=(const MVDouble &other);
  MVDouble &operator-=(const MVDouble &other);
  // </group>
  // Comparisons
  // <group>
  Bool operator==(const MVDouble &other) const;
  Bool operator!=(const MVDouble &other) const;
  Bool near(const MVDouble &other, Double tol = 1e-13) const;
  Bool nearAbs(const MVDouble &other, Double tol = 1e-13) const;
  // </group>
  
  //# General member functions
  // Tell me your type
  // <group>
  virtual uInt type() const;
  static void assure(const MeasValue &in);
  // </group>
  // Print data
  virtual void print(ostream &os) const;
  // Clone
  virtual MeasValue *clone() const;
  // Adjust value: taken from base class, a NOP.
  // Get the value in internal units
  virtual Vector<Double> getVector() const;
  // Set the value from internal units (set 0 for empty vector)
  virtual void putVector(const Vector<Double> &in);
   // Get the internal value as a <src>Vector<Quantity></src>. Usable in
  // records. The getXRecordValue() gets additional information for records.
  // Note that the Vectors could be empty.
  // <group>
  virtual Vector<Quantum<Double> > getRecordValue() const;
  // </group>
  // Set the internal value if correct values and dimensions
  virtual Bool putValue(const Vector<Quantum<Double> > &in);
 
private:
  //# Data
  // Value
  Double val;
};


} //# NAMESPACE CASA - END

#endif
