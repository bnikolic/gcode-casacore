//# BaseTabIter.cc: Base class for table iterator
//# Copyright (C) 1994,1995,1996
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

#include <aips/Tables/BaseTabIter.h>
#include <aips/Tables/TableIter.h>
#include <aips/Tables/RefTable.h>
#include <aips/Tables/TableColumn.h>
#include <aips/Utilities/Sort.h>
#include <aips/Tables/TableError.h>

// BaseTableIterator is the base class for the table iterators.
// It is a letter class of the envelope TableIterator.
//
// BaseTableIterator iterates by sorting the table in the required
// order and then creating a RefTable for each step containing the row
// numbers of the rows for that iteration step.


BaseTableIterator::BaseTableIterator (BaseTable* btp,
				      const Block<String>& keys,
				      const PtrBlock<ObjCompareFunc*>& cmpFunc,
				      const Block<Int>& order)
: lastVal_p (keys.nelements()),
  curVal_p  (keys.nelements()),
  lastRow_p (0),
  colPtr_p  (keys.nelements()),
  cmpFunc_p (cmpFunc),
  nrkeys_p  (keys.nelements())
{
    // Sort the table in order of the iteration keys.
    // DontCare is sorted as ascending.
    // Sort comparison functions are not used.
    // The passed in compare functions are for the iteration.
    Block<Int> ord(nrkeys_p, Sort::Ascending);
    for (uInt i=0; i<nrkeys_p; i++) {
	if (order[i] == TableIterator::Descending) {
	    ord[i] = Sort::Descending;
	}
    }
    sortTab_p = (RefTable*) (btp->sort (keys, cmpFunc_p, ord, Sort::HeapSort));
    sortTab_p->link();
    // Get the pointers to the BaseColumn object.
    // Get a buffer to hold the current and last value per column.
    for (i=0; i<nrkeys_p; i++) {
	colPtr_p[i] = sortTab_p->getColumn (keys[i]);
	colPtr_p[i]->allocIterBuf (lastVal_p[i], curVal_p[i], cmpFunc_p[i]);
    }
}


BaseTableIterator* BaseTableIterator::clone() const
{
    BaseTableIterator* newbti = new BaseTableIterator (*this);
    if (newbti == 0) {
	throw (AllocError ("BaseTableIterator::clone", 1));
    }
    return newbti;
}


BaseTableIterator::BaseTableIterator (const BaseTableIterator& that)
: lastVal_p (that.nrkeys_p),
  curVal_p  (that.nrkeys_p),
  lastRow_p (0),
  colPtr_p  (that.colPtr_p),
  cmpFunc_p (that.cmpFunc_p),
  nrkeys_p  (that.nrkeys_p)
{
    // Get the pointers to the BaseColumn object.
    // Get a buffer to hold the current and last value per column.
    for (uInt i=0; i<nrkeys_p; i++) {
	colPtr_p[i]->allocIterBuf (lastVal_p[i], curVal_p[i], cmpFunc_p[i]);
    }
    // Link against the table (ie. increase its ref.count).
    sortTab_p = that.sortTab_p;
    sortTab_p->link();
}


BaseTableIterator::~BaseTableIterator()
{
    // Delete the value buffers.
    for (uInt i=0; i<nrkeys_p; i++) {
	colPtr_p[i]->freeIterBuf (lastVal_p[i], curVal_p[i]);
    }
    // Unlink from the table and delete it.
    BaseTable::unlink (sortTab_p);
}


void BaseTableIterator::reset()
{
    lastRow_p = 0;
}


BaseTable* BaseTableIterator::next()
{
    uInt i;
    // Allocate a RefTable to represent the rows in the iteration group.
    RefTable* itp = sortTab_p->makeRefTable (False, 0);
    if (lastRow_p >= sortTab_p->nrow()) {
	return itp;                              // the end of the table
    }
    // Add the last found rownr to this iteration group.
    itp->addRownr (sortTab_p->rootRownr(lastRow_p));
    for (i=0; i<nrkeys_p; i++) {
	colPtr_p[i]->get (lastRow_p, lastVal_p[i]);
    }
    Bool match;
    uInt nr = sortTab_p->nrow();
    while (++lastRow_p < nr) {
	match = True;
	for (i=0; i<nrkeys_p; i++) {
	    colPtr_p[i]->get (lastRow_p, curVal_p[i]);
	    if (cmpFunc_p[i] (curVal_p[i], lastVal_p[i])  != 0) {
		match = False;
		break;
	    }
	}
	if (!match) {
	    break;
	}
	itp->addRownr (sortTab_p->rootRownr(lastRow_p));
    }
    return itp;
}
