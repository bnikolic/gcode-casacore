//# Sort.cc: Sort on one or more keys, ascending and/or descending
//# Copyright (C) 1993,1994,1995,1996
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

#include <aips/Utilities/Sort.h>
#include <aips/Utilities/ValType.h>
#include <aips/Utilities/Copy.h>
#include <aips/Utilities/SortError.h>
#include <aips/aips_exit.h>
#include <stdlib.h>                 // for rand
#include <aips/aips_enter.h>


SortKey::SortKey (const void* dat, ObjCompareFunc* cmpfunc, uInt inc,
		  int opt)
: data_p    (dat),
  cmpFunc_p (cmpfunc),
  incr_p    (inc),
  order_p   (opt)
{
    if (order_p != Sort::Descending) {
	order_p = Sort::Ascending;        // make sure order has correct value
    }
}

SortKey::SortKey (const SortKey& that)
: data_p    (that.data_p),
  cmpFunc_p (that.cmpFunc_p),
  incr_p    (that.incr_p),
  order_p   (that.order_p)
{ ; }

SortKey::~SortKey()
{}
void SortKey::cleanup()
    { this->SortKey::~SortKey(); }

SortKey& SortKey::operator= (const SortKey& that)
{
    if (this != &that) {
	data_p     = that.data_p;
	cmpFunc_p  = that.cmpFunc_p;
	incr_p     = that.incr_p;
	order_p    = that.order_p;
    }
    return *this;
}





Sort::Sort()
: nrkey_p (0),
  data_p  (0),
  size_p  (0)
    { ; }
Sort::Sort (const void* dat, uInt sz)
: nrkey_p (0),
  data_p  (dat),
  size_p  (sz)
    { ; }

Sort::~Sort()
{
    for (uInt i=0; i<nrkey_p; i++) {
	delete keys_p[i];
    }
}
void Sort::cleanup()
    { this->Sort::~Sort(); }


void Sort::sortKey (const void* dat, DataType dt, uInt inc, Order ord)
    { addKey (dat, dt, inc, ord); }
void Sort::sortKey (const void* dat, ObjCompareFunc* cmp, uInt inc, Order ord)
    { addKey (dat, cmp, inc, ord); }
void Sort::sortKey (uInt off, DataType dt, Order ord)
{
    if (data_p == 0) {
	throw (SortNoData());
    }
    addKey ((char*)data_p+off, dt, size_p, ord);
}
void Sort::sortKey (uInt off, ObjCompareFunc* cmp, Order ord)
{
    if (data_p == 0) {
	throw (SortNoData());
    }
    addKey ((char*)data_p+off, cmp, size_p, ord);
}


void Sort::addKey (const void* dat, DataType dt, uInt inc, int ord)
{
    uInt sz = ValType::getTypeSize (dt);
    if (inc != 0) {
	if (sz > inc) {
	    throw (SortInvIncr());
	}
	sz = inc;
    }
    addKey (dat, ValType::getCmpFunc(dt), sz, ord);
}


void Sort::addKey (const void* dat, ObjCompareFunc* cmp, uInt inc, int ord)
{
    if (nrkey_p >= keys_p.nelements()) {
	keys_p.resize (keys_p.nelements() + 32);
    }
    SortKey* skptr = new SortKey (dat, cmp, inc, ord);
    if (skptr == 0) {
	throw (AllocError ("SortKey",1));
    }
    keys_p[nrkey_p++] = skptr;
    skptr->makePermanent();        // Cleanup should not delete it
}


uInt Sort::sort (uInt nrrec, uInt*& inx, int opt) const
{
    // First build the index array of all records.
    // Allocate it if not allocated yet.
    if (inx == 0) {
	inx = new uInt[nrrec];
	if (inx == 0) {
	    throw (AllocError ("Sort-indices",nrrec));
	}
    }
    for (uInt i=0; i<nrrec; i++) {
	inx[i] = i;
    }
    return doSort (nrrec, inx, opt);               // do the sort
}


uInt Sort::sort (uInt nrrec, const uInt* inxin, uInt*& inx, int opt) const
{
    // First build the index array of all records.
    // Allocate it if not allocated yet.
    if (inx == 0) {
	inx = new uInt[nrrec];
	if (inx == 0) {
	    throw (AllocError ("Sort-indices",nrrec));
	}
    }
    objcopy (inx, inxin, nrrec);
    return doSort (nrrec, inx, opt);               // do the sort
}


uInt Sort::doSort (uInt nrrec, uInt* inx, int opt) const
{
    int nodup = opt & NoDuplicates;
    int type  = opt - nodup;
    switch (type) {
    case QuickSort:
	if (nodup) {
	    return quickSortNoDup (nrrec, inx);
	}
	return quickSort (nrrec,inx);
    case HeapSort:
	if (nodup) {
	    return heapSortNoDup (nrrec, inx);
	}
	return heapSort (nrrec,inx);
    case InsSort:
	if (nodup) {
	    return insSortNoDup (nrrec, inx);
	}
	return insSort (nrrec,inx);
    default:
	throw (SortInvOpt());
    }
    return 0;
}


uInt Sort::insSort (uInt nrrec, uInt* inx) const
{
    Int  j;
    uInt cur;
    for (Int i=1; i<nrrec; i++) {
	j   = i;
	cur = inx[i];
	while (--j>=0  &&  compare(inx[j], cur) <= 0) {
	    inx[j+1] = inx[j];
	}
	inx[j+1] = cur;
    }
    return nrrec;
}

uInt Sort::insSortNoDup (uInt nrrec, uInt* inx) const
{
    if (nrrec < 2) {
	return nrrec;                             // nothing to sort
    }
    Int  j, k;
    uInt cur;
    uInt nr = 1;
    int  cmp;
    for (Int i=1; i<nrrec; i++) {
	j   = nr;
	cur = inx[i];
	while (--j>=0  &&  (cmp = compare (inx[j], cur)) == 0) {
	}
	if (cmp == 2) {                         // no equal key
	    for (k=nr-1; k>j; k--) {
		inx[k+1] = inx[k];              // now shift to right
	    }
	    inx[j+1] = cur;                     // insert in right place
	    nr++;
	}
    }
    return nr;
}


uInt Sort::quickSort (uInt nrrec, uInt* inx) const
{
    // Use the quicksort algorithm and improvements as described
    // in "Algorithms in C" by R. Sedgewick.
    // Small subsets are not sorted with qksort anymore, but
    // thereafter with insertion sort.
    qkSort (nrrec, inx);
    return insSort (nrrec, inx);
}

uInt Sort::quickSortNoDup (uInt nrrec, uInt* inx) const
{
    qkSort (nrrec, inx);
    return insSortNoDup (nrrec, inx);
}


void Sort::qkSort (Int nr, uInt* inx) const
{
    // If the nr of elements to be sorted is less than N, it is
    // better not to use quicksort anymore (according to Sedgewick).
    // Take N=15, because that seems to work best after testing
    // N=5, 10, 15 and 20.
    if (nr <= 15) {
	return;
    }
    // According to Sedgewick it is best to use a random partition element
    // to avoid degenerated cases (if the data is already in order for example)
    // rand is not a particularly good random number generator, but good
    // enough for this purpose.
    // Put this element at the beginning of the array.
    Int p = rand() % nr;
    swap (0, p, inx);
    // Now shift all elements < partition-element to the left.
    // If an element is equal, shift every other element to avoid
    // degeneration. This trick is described by Jon Bentley in
    // UNIX Review, October 1992.
    // We do not have equal elements anymore (because of the stability
    // property introduced on 13-Feb-1995).
    Int j = 0;
    for (Int i=1; i<nr; i++) {
	if (compare (inx[0], inx[i]) <= 0) {
	    swap (i, ++j, inx);
	}
    }
    swap (0, j, inx);
    qkSort (j, inx);
    qkSort (nr-j-1, inx+j+1);
}


uInt Sort::heapSort (uInt nrrec, uInt* inx) const
{
    // Use the heapsort algorithm described by Jon Bentley in
    // UNIX Review, August 1992.
    inx--;
    for (Int j=nrrec/2; j>=1; j--) {
	siftDown (j, nrrec, inx);
    }
    for (j=nrrec; j>=2; j--) {
	swap (1, j, inx);
	siftDown (1, j-1, inx);
    }
    return nrrec;
}

uInt Sort::heapSortNoDup (uInt nrrec, uInt* inx) const
{
    heapSort (nrrec, inx);
    return insSortNoDup (nrrec, inx);
}

void Sort::siftDown (Int low, Int up, uInt* inx) const
{
    uInt sav = inx[low];
    Int c;
    for (Int i=low; (c=2*i)<=up; i=c) {
	if (c < up  &&  compare(inx[c+1], inx[c]) <= 0) {
	    c++;
	}
	inx[i] = inx[c];
    }
    inx[i] = sav;
    for ( ; (c=i/2)>=low; i=c) {
	if (compare (inx[i], inx[c]) > 0) {
	    break;
	}
	swap (c, i, inx);
    }
}


// Note that the block of SortKeys is defined as void*, to achieve
// that only 1 type of Block<pointer> is needed.
// Casting is perfectly save.
// The comparison functions return:
//   -1   when obj1 < obj2
//    0   when obj1 = obj2
//    1   when obj1 > obj2
// compare returns:
//    2   when data[i1],data[i2] is in correct order
//        (thus data[i1] < data[i2] for ascending sort)
//    1   when data is equal and indices are in order
//    0   when data is out of order
//   -1   when data is equal and indices are out of order
int Sort::compare (uInt i1, uInt i2) const
{
    int seq;
    SortKey* skp;
    for (Int i=0; i<nrkey_p; i++) {
	skp = (SortKey*)(keys_p[i]);        // cast from void* to SortKey*
	seq = skp->cmpFunc_p (
	              (char*)skp->data_p + i1*skp->incr_p,
	              (char*)skp->data_p + i2*skp->incr_p);
	if (seq == skp->order_p)
	    return 2;                       // in order
	if (seq != 0) {
	    return 0;                       // out-of-order
	}
    }
    // Equal keys, so return i1<i2 to maintain stability.
    if (i1<i2)
	return 1;                           // equal keys; in order
    return -1;                              // equal keys; out-of-order
}
