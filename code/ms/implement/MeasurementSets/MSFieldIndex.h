//# MSFieldIndex: index or lookup in a MeasurementSet FIELD subtable
//# Copyright (C) 2000,2001
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
//#
//# $Id$

#if !defined(AIPS_MSFIELDINDEX_H)
#define AIPS_MSFIELDINDEX_H

//# includes
#include <aips/aips.h>
#include <aips/MeasurementSets/MSField.h>
#include <aips/MeasurementSets/MSFieldColumns.h>
#include <aips/Arrays/Vector.h>
#include <aips/BasicSL/String.h>

//# forward declarations

// <summary>
// Class to handle lookup or indexing into a MS FIELD subtable
// </summary>

// <use visibility=export>
//
// <reviewed reviewer="" date="yyyy/mm/dd" tests="" demos="">
// </reviewed>

// <prerequisite>
//   <li> MeasurementSet
//   <li> MSField
// </prerequisite>
//
// <etymology>
// From "MeasurementSet", "FIELD subtable" and "index".
// </etymology>
//
// <synopsis>
// This class provides lookup and indexing into an MS FIELD
// subtable. These services include returning rows numbers
// (which for the FIELD subtable are FIELD_ID's) associated 
// with specific data in the subtable.
// </synopsis>
//
// <example>
// </example>
//
// <motivation>
// Collect together all subtable indexing and lookup for the
// FIELD subtable, for encapsulation and efficiency.
// </motivation>
//
// <thrown>
//    <li>
//    <li>
// </thrown>
//

class MSFieldIndex 
{
public:
  // Construct from an MS FIELD subtable
  MSFieldIndex(const MSField &field);

  // Null destructor
  virtual ~MSFieldIndex() {};

  // Look up FIELD_ID's for a given field name, or set of field names
  Vector<Int> matchFieldName(const String& name);
  Vector<Int> matchFieldName(const Vector<String>& names);

  // Look up FIELD_ID's for a given source id
  Vector<Int> matchSourceId(const Int& sourceId);
  Vector<Int> matchSourceId(const Vector<Int>& sourceIds);

private:
  // Disallow null constructor
  MSFieldIndex();

  // FIELD subtable column accessor
  ROMSFieldColumns msFieldCols_p;

  // Vector cache of field id's
  Vector<Int> fieldIds_p;
  Int nrows_p;
};

#endif
    
