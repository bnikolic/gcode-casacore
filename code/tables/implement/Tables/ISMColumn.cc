//# ISMColumn.cc: The Column of the Incremental Storage Manager
//# Copyright (C) 1996,1997
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

#include <aips/Tables/ISMColumn.h>
#include <aips/Tables/ISMBase.h>
#include <aips/Tables/ISMBucket.h>
#include <aips/Arrays/Array.h>
#include <aips/Utilities/ValType.h>
#include <aips/Utilities/Assert.h>
#include <aips/Utilities/Copy.h>
#include <aips/OS/CanonicalConversion.h>


ISMColumn::ISMColumn (ISMBase* parent, int dataType, uInt colnr)
: StManColumn   (dataType),
  stmanPtr_p    (parent),
  colnr_p       (colnr),
  nrelem_p      (1),
  fixedLength_p (0),
  startRow_p    (-1),
  endRow_p      (-1),
  lastValue_p   (0),
  lastRowPut_p  (0)
{}

ISMColumn::~ISMColumn()
{
    switch (dataType()) {
    case TpBool:
	delete [] (Bool*)lastValue_p;
	break;
    case TpUChar:
	delete [] (uChar*)lastValue_p;
	break;
    case TpShort:
	delete [] (Short*)lastValue_p;
	break;
    case TpUShort:
	delete [] (uShort*)lastValue_p;
	break;
    case TpInt:
	delete [] (Int*)lastValue_p;
	break;
    case TpUInt:
	delete [] (uInt*)lastValue_p;
	break;
    case TpFloat:
	delete [] (float*)lastValue_p;
	break;
    case TpDouble:
	delete [] (double*)lastValue_p;
	break;
    case TpComplex:
	delete [] (Complex*)lastValue_p;
	break;
    case TpDComplex:
	delete [] (DComplex*)lastValue_p;
	break;
    case TpString:
	delete [] (String*)lastValue_p;
	break;
    }
}

Bool ISMColumn::canAccessScalarColumn (Bool& reask) const
{
    reask = False;
    return False;
}

void ISMColumn::setShapeColumn (const IPosition& shape)
{
    nrelem_p = shape.product();
    shape_p  = shape;
}

uInt ISMColumn::ndim (uInt)
{
    return shape_p.nelements();
}
IPosition ISMColumn::shape (uInt)
{
    return shape_p;
}


void ISMColumn::addRow (uInt, uInt)
{
    //# Nothing to do.
}

void ISMColumn::remove (uInt bucketRownr, ISMBucket* bucket, uInt bucketNrrow)
{
    uInt inx, stint, endint, offset;
    // Get the index where to remove the new value.
    inx = bucket->getInterval (colnr_p, bucketRownr, bucketNrrow,
			       stint, endint, offset);
#ifdef AIPS_TRACE
    cout << "remove column " << colnr_p << ", row " << bucketRownr << " (nrelem="<<nrelem_p<<", leng="<<fixedLength_p<<")"<< endl;
    cout << "  Bucket start row " << bucketRownr << " (" << bucketNrrow << " rows)" << endl;
    cout << "  Interval " << stint << "-" << endint << " (row="<< bucketRownr<<", offset="<<offset<<")"<<endl;
#endif

    // Get bucket information needed to remove the data.
    Block<uInt>& rowIndex = bucket->rowIndex (colnr_p);
    Block<uInt>& offIndex = bucket->offIndex (colnr_p);
    uInt& nused = bucket->indexUsed (colnr_p);
    // Invalidate the last value read.
    startRow_p = -1;
    endRow_p   = -1;
    // We have to change the bucket, so let the cache set the dirty flag
    // for this bucket.
    stmanPtr_p->setBucketDirty();
    // When the row is single, remove the value by shifting left one value.
    if (stint == endint) {
	handleRemove (bucketRownr, bucket->get (offset));
	bucket->shiftLeft (inx, 1, rowIndex, offIndex, nused, fixedLength_p);
	//# We can also test if previous and next value are equal and
	//# shift left one more. However, that is not implemented.
    }
    // Decrement the row number for all following rows.
    for (uInt i=inx; i<nused; i++) {
	rowIndex[i]--;
    }
}


void ISMColumn::getBoolV (uInt rownr, Bool* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(Bool*)lastValue_p;
}
void ISMColumn::getuCharV (uInt rownr, uChar* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(uChar*)lastValue_p;
}
void ISMColumn::getShortV (uInt rownr, Short* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(Short*)lastValue_p;
}
void ISMColumn::getuShortV (uInt rownr, uShort* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(uShort*)lastValue_p;
}
void ISMColumn::getIntV (uInt rownr, Int* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(Int*)lastValue_p;
}
void ISMColumn::getuIntV (uInt rownr, uInt* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(uInt*)lastValue_p;
}
void ISMColumn::getfloatV (uInt rownr, float* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(float*)lastValue_p;
}
void ISMColumn::getdoubleV (uInt rownr, double* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(double*)lastValue_p;
}
void ISMColumn::getComplexV (uInt rownr, Complex* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(Complex*)lastValue_p;
}
void ISMColumn::getDComplexV (uInt rownr, DComplex* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(DComplex*)lastValue_p;
}
void ISMColumn::getStringV (uInt rownr, String* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = *(String*)lastValue_p;
}

void ISMColumn::getValue (uInt rownr, void* value)
{
    // Get the bucket with its row number boundaries.
    uInt bucketStartRow, bucketNrrow;
    ISMBucket* bucket = stmanPtr_p->getBucket (rownr, bucketStartRow,
					       bucketNrrow);
    // Get the interval in the bucket with its rownr boundaries.
    rownr -= bucketStartRow;
    uInt offset, stint, endint;
    bucket->getInterval (colnr_p, rownr, bucketNrrow, stint, endint, offset);
    // Get the value.
    // Set the start and end rownr for which this value is valid.
    readFunc_p (value, bucket->get (offset), nrcopy_p);
    startRow_p = bucketStartRow + stint;
    endRow_p   = bucketStartRow + endint;
}

void ISMColumn::putBoolV (uInt rownr, const Bool* value)
{
    putValue (rownr, value);
}
void ISMColumn::putuCharV (uInt rownr, const uChar* value)
{
    putValue (rownr, value);
}
void ISMColumn::putShortV (uInt rownr, const Short* value)
{
    putValue (rownr, value);
}
void ISMColumn::putuShortV (uInt rownr, const uShort* value)
{
    putValue (rownr, value);
}
void ISMColumn::putIntV (uInt rownr, const Int* value)
{
    putValue (rownr, value);
}
void ISMColumn::putuIntV (uInt rownr, const uInt* value)
{
    putValue (rownr, value);
}
void ISMColumn::putfloatV (uInt rownr, const float* value)
{
    putValue (rownr, value);
}
void ISMColumn::putdoubleV (uInt rownr, const double* value)
{
    putValue (rownr, value);
}
void ISMColumn::putComplexV (uInt rownr, const Complex* value)
{
    putValue (rownr, value);
}
void ISMColumn::putDComplexV (uInt rownr, const DComplex* value)
{
    putValue (rownr, value);
}
void ISMColumn::putStringV (uInt rownr, const String* value)
{
    putValue (rownr, value);
}

void ISMColumn::getArrayBoolV (uInt rownr, Array<Bool>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<Bool> (shape_p, (Bool*)lastValue_p, SHARE);
}

void ISMColumn::putArrayBoolV (uInt rownr, const Array<Bool>* value)
{
    Bool deleteIt;
    const Bool* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayuCharV (uInt rownr, Array<uChar>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<uChar> (shape_p, (uChar*)lastValue_p, SHARE);
}

void ISMColumn::putArrayuCharV (uInt rownr, const Array<uChar>* value)
{
    Bool deleteIt;
    const uChar* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayShortV (uInt rownr, Array<Short>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<Short> (shape_p, (Short*)lastValue_p, SHARE);
}

void ISMColumn::putArrayShortV (uInt rownr, const Array<Short>* value)
{
    Bool deleteIt;
    const Short* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayuShortV (uInt rownr, Array<uShort>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<uShort> (shape_p, (uShort*)lastValue_p, SHARE);
}

void ISMColumn::putArrayuShortV (uInt rownr, const Array<uShort>* value)
{
    Bool deleteIt;
    const uShort* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayIntV (uInt rownr, Array<Int>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<Int> (shape_p, (Int*)lastValue_p, SHARE);
}

void ISMColumn::putArrayIntV (uInt rownr, const Array<Int>* value)
{
    Bool deleteIt;
    const Int* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayuIntV (uInt rownr, Array<uInt>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<uInt> (shape_p, (uInt*)lastValue_p, SHARE);
}

void ISMColumn::putArrayuIntV (uInt rownr, const Array<uInt>* value)
{
    Bool deleteIt;
    const uInt* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayfloatV (uInt rownr, Array<float>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<float> (shape_p, (float*)lastValue_p, SHARE);
}

void ISMColumn::putArrayfloatV (uInt rownr, const Array<float>* value)
{
    Bool deleteIt;
    const float* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArraydoubleV (uInt rownr, Array<double>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<double> (shape_p, (double*)lastValue_p, SHARE);
}

void ISMColumn::putArraydoubleV (uInt rownr, const Array<double>* value)
{
    Bool deleteIt;
    const double* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayComplexV (uInt rownr, Array<Complex>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<Complex> (shape_p, (Complex*)lastValue_p, SHARE);
}

void ISMColumn::putArrayComplexV (uInt rownr, const Array<Complex>* value)
{
    Bool deleteIt;
    const Complex* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayDComplexV (uInt rownr, Array<DComplex>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<DComplex> (shape_p, (DComplex*)lastValue_p, SHARE);
}

void ISMColumn::putArrayDComplexV (uInt rownr, const Array<DComplex>* value)
{
    Bool deleteIt;
    const DComplex* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}
void ISMColumn::getArrayStringV (uInt rownr, Array<String>* value)
{
    if (isLastValueInvalid (rownr)) {
	getValue (rownr, lastValue_p);
    }
    *value = Array<String> (shape_p, (String*)lastValue_p, SHARE);
}

void ISMColumn::putArrayStringV (uInt rownr, const Array<String>* value)
{
    Bool deleteIt;
    const String* data = value->getStorage (deleteIt);
    putValue (rownr, data);
    value->freeStorage (data, deleteIt);
}


void ISMColumn::putValue (uInt rownr, const void* value)
{
    // Get the bucket and interval to which the row belongs.
    uInt bucketStartRow, bucketNrrow;
    ISMBucket* bucket = stmanPtr_p->getBucket (rownr, bucketStartRow,
					       bucketNrrow);
    uInt bucketRownr = rownr - bucketStartRow;
    uInt inx, stint, endint, offset;
    // Get the index where to add/replace the new value.
    // Note: offset gives the offset of the current value, which is often NOT
    //       the same as offIndex[inx] (usually it is offIndex[inx-1]).
    inx = bucket->getInterval (colnr_p, bucketRownr, bucketNrrow,
			       stint, endint, offset);

#ifdef AIPS_TRACE
    cout << "put column " << colnr_p << ", row " << rownr << " (nrelem="<<nrelem_p<<", leng="<<fixedLength_p<<")"<< endl;
    cout << "  Bucket start row " << bucketStartRow << " (" << bucketNrrow << " rows)" << endl;
    cout << "  Interval " << stint << "-" << endint << " (row="<< bucketRownr<<", offset="<<offset<<")"<<endl;
#endif

    // Get bucket information needed to write the data.
    Block<uInt>& rowIndex = bucket->rowIndex (colnr_p);
    Block<uInt>& offIndex = bucket->offIndex (colnr_p);
    uInt& nused = bucket->indexUsed (colnr_p);
    // Determine if the new row is after the last row ever put for this column.
    Bool afterLastRowPut = False;
    if (rownr >= lastRowPut_p) {

#ifdef AIPS_TRACE
        cout << " after last row " << lastRowPut_p<<endl;
#endif

	afterLastRowPut = True;
	lastRowPut_p    = rownr+1;
    }
    // Invalidate the last value read.
    startRow_p = -1;
    endRow_p   = -1;
    // Exit if new value equals current value.
    readFunc_p (lastValue_p, bucket->get (offset), nrcopy_p);
    if (compareValue (value, lastValue_p)) {

#ifdef AIPS_TRACE
        cout << " equal value" << endl;
#endif

	return;
    }
    // We have to write the value, so let the cache set the dirty flag
    // for this bucket.
    stmanPtr_p->setBucketDirty();
    // Get the temporary buffer from the storage manager.
    uInt lenData;
    char* buffer = stmanPtr_p->tempBuffer();
    // Test if the value equals the previous one if at the start of
    // the interval (or equals next value if at the end of the interval).
    // Take care if row is first (or last) row in bucket.
    // The index of the next value is inx+1 when the row is the starting
    // row of the interval.
    Bool equalPrev = False;
    Bool equalNext = False;
    uInt nextInx = inx;
    if (bucketRownr == stint) {
	nextInx++;
	if (bucketRownr > 0) {
	    readFunc_p (lastValue_p, bucket->get (offIndex[inx-1]),
			nrcopy_p);
	    if (compareValue (value, lastValue_p)) {
		equalPrev = True;
	    }
	}
    }
    if (bucketRownr == endint) {
	if (bucketRownr < bucketNrrow-1) {
	    readFunc_p (lastValue_p, bucket->get (offIndex[nextInx]),
			nrcopy_p);
	    if (compareValue (value, lastValue_p)) {
		equalNext = True;
	    }
	}
    }
    // If the row is higher than the last row ever put for this column,
    // the value is valid for all rows from this row on.
    // If it is the first row in the bucket, replace the value.
    // If the new value equals the previous one, combine them.
    // Otherwise add it to this bucket.
    // In any case put the value in all further buckets.
    if (afterLastRowPut) {
	lenData = writeFunc_p (buffer, value, nrcopy_p);
	if (bucketRownr == 0) {
	    replaceData (bucket, bucketStartRow, bucketNrrow, bucketRownr,
			 offIndex[inx], buffer, lenData);
	}else{
	    if (equalPrev) {

#ifdef AIPS_TRACE
		cout << " equal prev";
#endif

		bucket->shiftLeft (inx, 1, rowIndex, offIndex, nused,
				   fixedLength_p);
	    }else{
		addData (bucket, bucketStartRow, bucketNrrow,
			 bucketRownr, inx, buffer, lenData, True);
	    }
	}
        putFromRow (rownr, buffer, lenData);
	return;
    }
    // When the new value matches previous and next, we can contract the
    // values by removing 2 of them.
    if (equalPrev && equalNext) {

#ifdef AIPS_TRACE
        cout << " equal prev and next"<<endl;
#endif

	bucket->shiftLeft (inx, 2, rowIndex, offIndex, nused, fixedLength_p);
	return;
    }
    // Determine if the value is the only one in the interval.
    // When the row to change is the first of the interval, increment
    // the interval start.
    Bool single = ToBool(stint==endint);
//#    if (!single  &&  bucketRownr == stint) {
//#	rowIndex[inx]++;
//#    }
    // When matching the previous, combine with previous interval
    // (which is already done by incrementing the rowIndex above).
    // When it was a single value, contract the intervals.
    if (equalPrev) {

#ifdef AIPS_TRACE
        cout << " equal prev";
#endif

	if (single) {

#ifdef AIPS_TRACE
	    cout << " and single";
#endif

	    bucket->shiftLeft (inx, 1, rowIndex, offIndex, nused,
			       fixedLength_p);
	}else{
	    rowIndex[inx]++;
	}

#ifdef AIPS_TRACE
	cout << endl;
#endif

	return;
    }
    // When equal to next value, act similarly as above.
    if (equalNext) {

#ifdef AIPS_TRACE
        cout << " equal next";
#endif

	if (single) {

#ifdef AIPS_TRACE
	    cout << " and single";
#endif

	    bucket->shiftLeft (inx, 1, rowIndex, offIndex, nused,
			       fixedLength_p);
	}
	rowIndex[inx]--;

#ifdef AIPS_TRACE
	cout << endl;
#endif

	return;
    }
    // We have to add or replace the new data value.
    // When the value is single, simply replace it.
    // This will also update the offset value when needed.
    if (single) {
	lenData = writeFunc_p (buffer, value, nrcopy_p);
	replaceData (bucket, bucketStartRow, bucketNrrow, bucketRownr,
		     offIndex[inx], buffer, lenData);
	return;
    }
    // Add the data item.
    // When the new value is in the middle of the interval, the
    // original value has to be duplicated. Give a derived class
    // the opportunity to handle the copy.
    // Do this before inserting the new value. Otherwise the new
    // value may get at the end of the bucket and fill up the bucket.
    // Thereafter inserting the duplicate results in a split and the new
    // value may get promoted to the new bucket iso. the original.
    if (bucketRownr > stint  &&  bucketRownr < endint) {
	lenData = writeFunc_p (buffer, lastValue_p, nrcopy_p);
	addData (bucket, bucketStartRow, bucketNrrow,
		 bucketRownr+1, inx, buffer, lenData);
	handleCopy (rownr, buffer);
	putValue (rownr, value);
    }else{
	lenData = writeFunc_p (buffer, value, nrcopy_p);
	addData (bucket, bucketStartRow, bucketNrrow,
		 bucketRownr, inx, buffer, lenData);
    }
}

void ISMColumn::putFromRow (uInt rownr, const char* data, uInt lenData)
{
    // Skip the first bucket, because that is the one containing the
    // row just written.
    // Note that the previous write may have resulted in a bucket split.
    // So the previously calculated end of the bucket may not be right
    // anymore. Therefore we start at the given row and skip that bucket.
#ifdef AIPS_TRACE
    cout << "  putFromRow";
#endif

    ISMBucket* bucket;
    uInt bucketNrrow;
    uInt cursor = 0;
    bucket = stmanPtr_p->nextBucket (cursor, rownr, bucketNrrow);
    // Loop through all buckets from the given row on.
    // Replace the starting value in them.
    while ((bucket = stmanPtr_p->nextBucket (cursor, rownr, bucketNrrow))
	                                                           != 0) {
#ifdef AIPS_TRACE
        cout << "," << rownr;
#endif

	// Set the dirty flag for this bucket.
	stmanPtr_p->setBucketDirty();
	replaceData (bucket, rownr, bucketNrrow, 0,
		     bucket->getOffset (colnr_p, 0), data, lenData);
	// The value has been duplicated; give a derived class the
	// opportunity to handle it.
	handleCopy (rownr, data);
    }

#ifdef AIPS_TRACE
        cout << endl;
#endif
}

void ISMColumn::putData (ISMBucket* bucket, uInt bucketStartRow,
			 uInt bucketNrrow, uInt bucketRownr,
			 const char* data, uInt lenData,
			 Bool afterLastRow, Bool canSplit)
{
    // Determine the index.
    uInt inx, dum1, dum2, dum3;
    inx = bucket->getInterval (colnr_p, bucketRownr, 0, dum1, dum2, dum3);
    if (afterLastRow  &&  bucketRownr == 0) {
	Block<uInt>& offIndex = bucket->offIndex (colnr_p);
	replaceData (bucket, bucketStartRow, bucketNrrow, bucketRownr,
		     offIndex[inx], data, lenData, canSplit);
    }else{
	addData (bucket, bucketStartRow, bucketNrrow, bucketRownr, inx,
		 data, lenData, afterLastRow, canSplit);
    }
}

void ISMColumn::replaceData (ISMBucket* bucket, uInt bucketStartRow,
			     uInt bucketNrrow, uInt bucketRownr, uInt& offset,
			     const char* data, uInt lenData, Bool canSplit)
{
    // Replacing a value means removing the old value.
    // So give the opportunity to handle a removal before the
    // actual replace is done.
    // When the new value fits in the bucket, it can simply be replaced.
    uInt oldLeng = bucket->getLength (fixedLength_p, bucket->get (offset));
    if (bucket->canReplaceData (lenData, oldLeng)) {
	handleRemove (bucketRownr, bucket->get (offset));
	bucket->replaceData (offset, data, lenData, oldLeng);
	return;
    }
    // The bucket is too small, so split it in the middle (if allowed).
    AlwaysAssert (canSplit, AipsError);
    ISMBucket* left;
    ISMBucket* right;
    Block<Bool> duplicated;
    uInt splitRownr = bucket->split (left, right, duplicated,
				     bucketStartRow, bucketNrrow,
				     colnr_p, bucketRownr, lenData - oldLeng);

#ifdef AIPS_TRACE
    cout << "  replace split at rownr "<<splitRownr<<endl;
#endif

    handleSplit (*right, duplicated);
    // Replace the bucket by the left one (its starting row is the same).
    bucket->copy (*left);
    delete left;
    // Replace the data in the correct part.
    if (bucketRownr >= splitRownr) {
	bucket = right;
	bucketRownr -= splitRownr;
    }
    uInt& offs = bucket->getOffset (colnr_p, bucketRownr);
    handleRemove (bucketRownr, bucket->get (offs));
    bucket->replaceData (offs, data, lenData, oldLeng);
    // Add the right bucket to the index.
    stmanPtr_p->addBucket (splitRownr + bucketStartRow, right);
}

Bool ISMColumn::addData (ISMBucket* bucket, uInt bucketStartRow,
			 uInt bucketNrrow, uInt bucketRownr, uInt inx,
			 const char* data, uInt lenData,
			 Bool afterLastRow, Bool canSplit)
{
    // When the value fits in the bucket, it can simply be added.
    if (bucket->canAddData (lenData)) {
	bucket->addData (colnr_p, bucketRownr, inx, data, lenData);
	return False;
    }
    // The bucket is too small, so split it in the middle (if allowed).
    AlwaysAssert (canSplit, AipsError);
    ISMBucket* left;
    ISMBucket* right;
    Block<Bool> duplicated;
    uInt splitRownr = bucket->split (left, right, duplicated,
				     bucketStartRow, bucketNrrow,
				     colnr_p, bucketRownr, lenData);

#ifdef AIPS_TRACE
    cout << "  add split at rownr "<<splitRownr<<endl;
#endif

    handleSplit (*right, duplicated);
    // Replace the bucket by the left one (its starting row is the same).
    bucket->copy (*left);
    delete left;
    // Add the data to the correct part.
    uInt startRow = bucketStartRow;
    uInt nrrow    = splitRownr;
    if (bucketRownr >= splitRownr) {
	bucket = right;
	bucketRownr -= splitRownr;
	startRow    += splitRownr;
	nrrow        = bucketNrrow - splitRownr;
    }
    // The next put cannot split anymore.
    putData (bucket, startRow, nrrow, bucketRownr, data, lenData,
	     afterLastRow, False);
    // Add the right bucket to the index.
    stmanPtr_p->addBucket (splitRownr + bucketStartRow, right);
    return True;
}

void ISMColumn::handleCopy (uInt rownr, const char*)
{
#ifdef AIPS_TRACE
    cout << "   handleCopy for row " << rownr
	 << ", column " << colnr_p << endl;
#endif
}
void ISMColumn::handleRemove (uInt rownr, const char*)
{
#ifdef AIPS_TRACE
    cout << "   handleRemove for row " << rownr
	 << ", column " << colnr_p << endl;
#endif
}

void ISMColumn::handleSplit (ISMBucket& bucket, const Block<Bool>& duplicated)
{
    // Loop through all columns.
    // When the split duplicated a value, handle the copied value.
    uInt nrcol = stmanPtr_p->ncolumn();
    for (uInt i=0; i<nrcol; i++) {
	if (duplicated[i]) {
	    uInt offset = bucket.getOffset (i, 0);
	    stmanPtr_p->getColumn(i).handleCopy (0, bucket.get (offset));
	}
    }
}


Bool ISMColumn::compareValue (const void* val1, const void* val2) const
{
    const char* v1 = (const char*)val1;
    const char* v2 = (const char*)val2;
    for (uInt i=0; i<nrelem_p; i++) {
	if (compareFunc_p (v1, v2)  !=  0) {
	    return False;
	}
	v1 += typeSize_p;
	v2 += typeSize_p;
    }
    return True;
}

void ISMColumn::init()
{
    DataType dt = (DataType)dataType();
    typeSize_p = ValType::getTypeSize (dt);
    Bool asCanonical = stmanPtr_p->asCanonical();
    nrcopy_p = nrelem_p;
    if (dt == TpString) {
	fixedLength_p = 0;
    } else if (dt == TpBool) {
	fixedLength_p = (nrelem_p + 7) / 8;
    }else{
	if (asCanonical) {
	    fixedLength_p = ValType::getCanonicalSize (dt);
	    uInt nrel;
	    ValType::getCanonicalFunc (dt, readFunc_p, writeFunc_p, nrel);
	    nrcopy_p *= nrel;
	}else{
	    fixedLength_p = typeSize_p;
	    readFunc_p = writeFunc_p = Conversion::valueCopy;
	    nrcopy_p *= typeSize_p;
	}
	fixedLength_p *= nrelem_p;
    }
    switch (dt) {
    case TpBool:
	{
	    readFunc_p  = Conversion::bitToBool;
	    writeFunc_p = Conversion::boolToBit;
	    compareFunc_p = ObjCompare<Bool>::compare;
	    lastValue_p   = new Bool [nrelem_p];
	    Bool undef;
	    ValType::getUndef (&undef);
	    objset ((Bool*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpUChar:
	{
	    compareFunc_p = ObjCompare<uChar>::compare;
	    lastValue_p   = new uChar [nrelem_p];
	    uChar undef;
	    ValType::getUndef (&undef);
	    objset ((uChar*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpShort:
	{
	    compareFunc_p = ObjCompare<Short>::compare;
	    lastValue_p   = new Short [nrelem_p];
	    Short undef;
	    ValType::getUndef (&undef);
	    objset ((Short*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpUShort:
	{
	    compareFunc_p = ObjCompare<uShort>::compare;
	    lastValue_p   = new uShort [nrelem_p];
	    uShort undef;
	    ValType::getUndef (&undef);
	    objset ((uShort*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpInt:
	{
	    compareFunc_p = ObjCompare<Int>::compare;
	    lastValue_p   = new Int [nrelem_p];
	    Int undef;
	    ValType::getUndef (&undef);
	    objset ((Int*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpUInt:
	{
	    compareFunc_p = ObjCompare<uInt>::compare;
	    lastValue_p   = new uInt [nrelem_p];
	    uInt undef;
	    ValType::getUndef (&undef);
	    objset ((uInt*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpFloat:
	{
	    compareFunc_p = ObjCompare<float>::compare;
	    lastValue_p   = new float [nrelem_p];
	    float undef;
	    ValType::getUndef (&undef);
	    objset ((float*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpDouble:
	{
	    compareFunc_p = ObjCompare<double>::compare;
	    lastValue_p   = new double [nrelem_p];
	    double undef;
	    ValType::getUndef (&undef);
	    objset ((double*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpComplex:
	{
	    compareFunc_p = ObjCompare<Complex>::compare;
	    lastValue_p   = new Complex [nrelem_p];
	    Complex undef;
	    ValType::getUndef (&undef);
	    objset ((Complex*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpDComplex:
	{
	    compareFunc_p = ObjCompare<DComplex>::compare;
	    lastValue_p   = new DComplex [nrelem_p];
	    DComplex undef;
	    ValType::getUndef (&undef);
	    objset ((DComplex*)lastValue_p, undef, nrelem_p);
	}
	break;
    case TpString:
	{
	    if (asCanonical) {
		readFunc_p  = readString;
		writeFunc_p = writeString;
	    }else{
		readFunc_p  = copyToString;
		writeFunc_p = copyFromString;
	    }
	    compareFunc_p = ObjCompare<String>::compare;
	    lastValue_p   = new String [nrelem_p];
	    String undef;
	    ValType::getUndef (&undef);
	    objset ((String*)lastValue_p, undef, nrelem_p);
	}
	break;
    default:
	AlwaysAssert (0, AipsError);
    }
    AlwaysAssert (lastValue_p != 0, AipsError);
}

void ISMColumn::doCreate (ISMBucket* bucket)
{
    init();
    char* buffer = stmanPtr_p->tempBuffer();
    uInt leng = writeFunc_p (buffer, lastValue_p, nrcopy_p);
    bucket->addData (colnr_p, 0, 0, buffer, leng);
}

void ISMColumn::getFile (uInt nrrow)
{
    init();
    lastRowPut_p = nrrow;
}
void ISMColumn::flush (uInt)
{}
void ISMColumn::reopenRW()
{}


uInt ISMColumn::copyuInt (void* out, const void* in, uInt nvalues)
{
    nvalues *= sizeof(uInt);
    memcpy (out, in, nvalues);
    return nvalues;
}
Conversion::ValueFunction* ISMColumn::getReaduInt (Bool asCanonical)
{
    if (asCanonical) {
	return CanonicalConversion::getToLocal ((uInt*)0);
    }
    return ISMColumn::copyuInt;
}
Conversion::ValueFunction* ISMColumn::getWriteuInt (Bool asCanonical)
{
    if (asCanonical) {
	return CanonicalConversion::getFromLocal ((uInt*)0);
    }
    return ISMColumn::copyuInt;
}


uInt ISMColumn::fromString (void* out, const void* in, uInt n,
			    Conversion::ValueFunction* writeLeng)
{
    // The first entry represents the length of the entire object.
    // If there is only one string, it is the length of the string.
    // If there are multiple strings, it is the length of all strings
    // (including the lengths of their lengths).
    // Note that the length of itself is also included.
    char* buf = (char*)out;
    uInt leng = 0;
    uInt strleng;
    if (n > 1) {
	leng = writeLeng (buf, &leng, 1);
    }
    for (uInt i=0; i<n; i++) {
	const String& str = ((const String*)in)[i];
	strleng = str.length();
	leng += writeLeng (buf + leng, &strleng, 1);
	memcpy (buf + leng, str.chars(), strleng);
	leng += strleng;
    }
    writeLeng (buf, &leng, 1);
    return leng;
}
uInt ISMColumn::toString (void* out, const void* in, uInt n,
			  Conversion::ValueFunction* readLeng)
{
    const char* buf = (const char*)in;
    uInt strleng;
    uInt leng = readLeng (&strleng, buf, 1);
    strleng -= leng;
    for (uInt i=0; i<n; i++) {
	if (n > 1) {
	    leng += readLeng (&strleng, buf + leng, 1);
	}
	String& str = ((String*)out)[i];
	str.alloc (strleng);
	char* var = (char*)(str.chars());
	memcpy (var, buf + leng, strleng);
	var[strleng] = '\0';
	leng += strleng;
    }
    return leng;
}
uInt ISMColumn::copyFromString (void* out, const void* in, uInt n)
{
    return fromString (out, in, n, copyuInt);
}
uInt ISMColumn::copyToString (void* out, const void* in, uInt n)
{
    return toString (out, in, n, copyuInt);
}
uInt ISMColumn::writeString (void* out, const void* in, uInt n)
{
    return fromString (out, in, n, CanonicalConversion::fromLocalUInt);
}
uInt ISMColumn::readString (void* out, const void* in, uInt n)
{
    return toString (out, in, n, CanonicalConversion::toLocalUInt);
}
