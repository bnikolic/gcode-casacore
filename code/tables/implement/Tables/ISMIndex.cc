//# ISMIndex.cc: The Index of the Incremental Storage Manager
//# Copyright (C) 1996
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

//# Includes
#include <aips/Tables/ISMIndex.h>
#include <aips/Containers/BlockIO.h>
#include <aips/Utilities/BinarySearch.h>
#include <aips/IO/AipsIO.h>
#include <aips/Utilities/Assert.h>
#include <aips/Exceptions/Error.h>


ISMIndex::ISMIndex (ISMBase* parent)
: stmanPtr_p (parent),
  nused_p    (1),
  rows_p     (2, (uInt)0),
  bucketNr_p (1, (uInt)0)
{}

ISMIndex::ISMIndex (ISMBase* parent, AipsIO& os)
: stmanPtr_p (parent),
  nused_p    (1),
  rows_p     (2, (uInt)0),
  bucketNr_p (1, (uInt)0)
{
    // Read the index.
    get (os);
}

ISMIndex::~ISMIndex()
{}

void ISMIndex::get (AipsIO& os)
{
    os.getstart ("ISMIndex");
    os >> nused_p;
    getBlock (os, rows_p);
    getBlock (os, bucketNr_p);
    os.getend();
}

void ISMIndex::put (AipsIO& os)
{
    os.putstart ("ISMIndex", 1);
    os << nused_p;
    putBlock (os, rows_p, nused_p + 1);
    putBlock (os, bucketNr_p, nused_p);
    os.putend();
}

void ISMIndex::addBucketNr (uInt rownr, uInt bucketNr)
{
    if (nused_p >= bucketNr_p.nelements()) {
	rows_p.resize (nused_p + 64 + 1);
	bucketNr_p.resize (nused_p + 64);
    }
    Bool found;
    uInt index = binarySearchBrackets (found, rows_p, rownr, (uInt)nused_p);
    AlwaysAssert (!found, AipsError);
    objmove (&rows_p[index+1], &rows_p[index], nused_p + 1 - index);
    objmove (&bucketNr_p[index+1], &bucketNr_p[index], nused_p - index);
    rows_p[index] = rownr;
    bucketNr_p[index] = bucketNr;
    nused_p++;
}

void ISMIndex::addRow (uInt nrrow)
{
    rows_p[nused_p] += nrrow;
}

void ISMIndex::removeRow (uInt rownr)
{
    // Decrement the row number for all intervals after the row
    // to be removed.
    Bool found;
    uInt index = binarySearchBrackets (found, rows_p, rownr, (uInt)nused_p);
    for (uInt i=index+1; i<=nused_p; i++) {
	rows_p[i]--;
    }
    // Remove the entire interval when no row is left.
    // This results in a wasted bucket, but this situation should
    // in practice never occur.
    if (rows_p[index] == rows_p[index+1]) {
	objmove (&rows_p[index], &rows_p[index+1], nused_p - index);
	objmove (&bucketNr_p[index], &bucketNr_p[index+1], nused_p - index-1);
    }
}

uInt ISMIndex::getIndex (uInt rownr) const
{
    // If no exact match, the interval starts at the previous index.
    Bool found;
    uInt index = binarySearchBrackets (found, rows_p, rownr, (uInt)nused_p+1);
    if (!found) {
	index--;
    }
    AlwaysAssert (index <= nused_p, AipsError);
    return index;
}

uInt ISMIndex::getBucketNr (uInt rownr, uInt& bucketStartRow,
			    uInt& bucketNrrow) const
{
    uInt index = getIndex (rownr);
    bucketStartRow = rows_p[index];
    bucketNrrow    = rows_p[index+1] - bucketStartRow;
    return bucketNr_p[index];
}

Bool ISMIndex::nextBucketNr (uInt& cursor, uInt& bucketStartRow,
			     uInt& bucketNrrow, uInt& bucketNr) const
{
    // When first time, get the index of the bucket containing the row.
    // End the iteration when the first row is past the end.
    if (cursor == 0) {
	if (bucketStartRow >= rows_p[nused_p]) {
	    return False;
	}
	cursor = getIndex (bucketStartRow);
    }else{
	// Not the first time.
	// End the iteration when no more buckets.
	if (cursor >= nused_p) {
	    return False;
	}
    }
    bucketStartRow = rows_p[cursor];
    bucketNrrow    = rows_p[cursor+1] - bucketStartRow;
    bucketNr       = bucketNr_p[cursor++];
    return True;
}
