//# tMSDataDescBuffer.cc:
//# Copyright (C) 2000
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
#include <trial/MeasurementSets/MSDataDescBuffer.h>
#include <aips/Arrays/Vector.h>
#include <aips/Exceptions/Error.h>
#include <aips/MeasurementSets/NewMSDataDescription.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/String.h>
#include <iostream.h>

int main() {
  const String filename = "tMSDataDescBuffer_tmp.table";
  try {
    // Check the default constructor
    MSDataDescBuffer newBuffer;
    // test the ok function.
    AlwaysAssert(newBuffer.ok(), AipsError);
    // test the addRow & nrow functions.
    AlwaysAssert(newBuffer.nrow() == 0, AipsError);
    newBuffer.addRow(20);
    AlwaysAssert(newBuffer.ok(), AipsError);
    AlwaysAssert(newBuffer.nrow() == 20, AipsError);
    {
      MSDataDescBuffer buffer;
      { // test the addRow & nrow functions.
  	AlwaysAssert(buffer.nrow() == 0, AipsError);
  	buffer.addRow(5);
  	AlwaysAssert(buffer.nrow() == 5, AipsError);
      }
      { // test the spectralWindowId functions.
  	AlwaysAssert(buffer.spectralWindowId()(0) == -1, AipsError);
  	AlwaysAssert(buffer.spectralWindowId()(4) == -1, AipsError);
  	buffer.spectralWindowId().put(0, 0);
  	buffer.spectralWindowId().put(4, 1);
      }
      { // test the polarizationId functions.
 	AlwaysAssert(buffer.polarizationId()(0) == -1, AipsError);
 	AlwaysAssert(buffer.polarizationId()(4) == -1, AipsError);
 	buffer.polarizationId().put(0, 1);
 	buffer.polarizationId().put(4, 3);
      }
      { // test the flagRow functions.
  	AlwaysAssert(buffer.flagRow()(0) == False, AipsError);
  	AlwaysAssert(buffer.flagRow()(4) == False, AipsError);
  	buffer.flagRow().put(4, True);
      }
      { // Check the assignment operator & copy constructor
   	MSDataDescBuffer otherBuffer(buffer);
   	AlwaysAssert(otherBuffer.ok(), AipsError);
   	AlwaysAssert(otherBuffer.nrow() == 5, AipsError);
	newBuffer = otherBuffer;
   	AlwaysAssert(newBuffer.ok(), AipsError);
	// Check the reference semantics by adding data here and seeing if it
	// is mirrored into the newBuffer object.
	buffer.spectralWindowId().put(1, 100);
	buffer.polarizationId().put(1, 101);
	buffer.flagRow().put(1, True);
	// Save the buffer to disk
	buffer.save(filename, True);
      }
    }
    { // check the data has not been lost.
      AlwaysAssert(newBuffer.nrow() == 5, AipsError);
      AlwaysAssert(newBuffer.spectralWindowId()(0) ==  0, AipsError);
      AlwaysAssert(newBuffer.spectralWindowId()(4) ==  1, AipsError);
      AlwaysAssert(newBuffer.polarizationId()(0) ==  1, AipsError);
      AlwaysAssert(newBuffer.polarizationId()(4) ==  3, AipsError);
      AlwaysAssert(newBuffer.flagRow()(0) == False, AipsError);
      AlwaysAssert(newBuffer.flagRow()(4) == True, AipsError);
      // check the reference semantics
      AlwaysAssert(newBuffer.spectralWindowId()(1) ==  100, AipsError);
      AlwaysAssert(newBuffer.polarizationId()(1) ==  101, AipsError);
      AlwaysAssert(newBuffer.flagRow()(1) == True, AipsError);
    }
    { // Check the isValid functions
      AlwaysAssert(newBuffer.isValid(True) == False, AipsError);
      AlwaysAssert(newBuffer.isValid(3u) == False, AipsError);
      AlwaysAssert(newBuffer.isValid(4u) == True, AipsError);
      AlwaysAssert(newBuffer.isValid() == False, AipsError);
    }
    { // Check the match functions
    }
  }
  catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  }
  try {
    // Check that the Table ended up on disk (after the save function).
    NewMSDataDescription ms(filename);
    ms.markForDelete();
  }
  catch (AipsError x) {
    cerr << x.getMesg() << endl;
    cout << "FAIL" << endl;
    return 1;
  }
  cout << "OK" << endl;
}

// Local Variables: 
// compile-command: "gmake OPTLIB=1 XLIBLIST=0 tMSDataDescBuffer"
// End: 
