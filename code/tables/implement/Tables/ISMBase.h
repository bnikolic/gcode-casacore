//# ISMBase.h: Base class of the Incremental Storage Manager
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

#if !defined(AIPS_ISMBASE_H)
#define AIPS_ISMBASE_H

#if defined(_AIX)
#pragma implementation ("ISMBase.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/DataManager.h>
#include <aips/Containers/Block.h>

//# Forward declarations
class BucketCache;
class BucketFile;
class ISMBucket;
class ISMIndex;
class ISMColumn;
imported class ostream;


// <summary>
// Base class of the Incremental Storage Manager
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="tIncrementalStMan.cc">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=IncrementalStMan>IncrementalStMan</linkto>
//   <li> <linkto class=ISMColumn>ISMColumn</linkto>
// </prerequisite>

// <etymology>
// ISMBase is the base class of the Incremental Storage Manager.
// </etymology>

// <synopsis>
// The behaviour of this class is described in
// <linkto class="IncrementalStMan:description">IncrementalStMan</linkto>.

// <motivation>
// The public interface of ISMBase is quite large, because the other
// internal ISM classes need these functions. To have a class with a
// minimal interface for the normal user, class <src>IncrementalStMan</src>
// is derived from it.
// <br>IncrementalStMan needs an isA- instead of hasA-relation to be
// able to bind columns to it in class <linkto class=SetupNewTable>
// SetupNewTable</linkto>.
// </motivation>

// <todo asof="$DATE:$">
//# A List of bugs, limitations, extensions or planned refinements.
//   <li> Removed AipsIO argument from open and close.
// </todo>


class ISMBase: public DataManager
{
public:
    // Create an incremental storage manager without a name.
    // The bucket size has to be given in bytes and the cache size in buckets.
    // The bucket size is checked or calculated as described in
    // IncrementalStMan.h.
    explicit ISMBase (uInt bucketSize = 0, Bool checkBucketSize = True,
		      uInt cacheSize = 1);

    // Create an incremental storage manager with the given name.
    // The bucket size has to be given in bytes and the cache size in buckets.
    // The bucket size is checked or calculated as described in
    // IncrementalStMan.h.
   ISMBase (const String& dataManagerName,
	    uInt bucketSize, Bool checkBucketSize, uInt cacheSize);

    ~ISMBase();

    // Clone this object.
    // It does not clone ISMColumn objects possibly used.
    // The caller has to delete the newly created object.
    virtual DataManager* clone() const;

    // Get the type name of the data manager (i.e. IncrementalStMan).
    virtual String dataManagerType() const;

    // Get the name given to the storage manager (in the constructor).
    virtual String dataManagerName() const;

    // Set the cache size (in buckets).
    void setCacheSize (uInt cacheSize);

    // Get the current cache size (in buckets).
    uInt cacheSize() const;

    // Clear the cache used by this storage manager.
    // It will flush the cache as needed and remove all buckets from it.
    void clearCache();

    // Show the statistics of all caches used.
    void showCacheStatistics (ostream& os) const;

    // Get the bucket size (in bytes).
    uInt bucketSize() const;

    // Have the data to be stored in canonical format?
    Bool asCanonical() const;

    // Get the size of a uInt in external format (can be canonical or local).
    uInt uIntSize() const;

    // Get the bucket containing the given row.
    // Also return the first and last row of that bucket.
    // The bucket object is created and deleted by the caching mechanism.
    ISMBucket* getBucket (uInt rownr, uInt& bucketStartRow,
			  uInt& bucketNrrow);

    // Get the next bucket.
    // cursor=0 indicates the start of the iteration.
    // The first bucket returned is the bucket containing the rownr
    // given in bucketStartRow.
    // After each iteration BucketStartRow and bucketNrrow are set.
    // A 0 is returned when no more buckets.
    // The bucket object is created and deleted by the caching mechanism.
    ISMBucket* nextBucket (uInt& cursor, uInt& bucketStartRow,
			   uInt& bucketNrrow);

    // Get a pointer to the value for the given column and row.
    // It also fills the start/end for the row-interval of this value.
//    void* get (uInt colnr, uInt rownr);

    // Get access to the temporary buffer.
    char* tempBuffer() const;

    // Get a unique column number for the column
    // (it is only unique for this storage manager).
    // This is used by ISMColumnIndArr to create a unique file name.
    uInt uniqueNr();

    // Get the number of rows in this storage manager.
    uInt nrow() const;

    // Can the storage manager add rows? (yes)
    virtual Bool canAddRow() const;

    // Can the storage manager delete rows? (yes)
    virtual Bool canRemoveRow() const;

    // Can the storage manager add columns? (not yet)
    virtual Bool canAddColumn() const;

    // Can the storage manager delete columns? (not yet)
    virtual Bool canRemoveColumn() const;

    // Make the object from the type name string.
    // This function gets registered in the DataManager "constructor" map.
    // The caller has to delete the object.
    static DataManager* makeObject (const String& dataManagerType);

    // Get access to the given column.
    ISMColumn& getColumn (uInt colnr);

    // Add a bucket to the storage manager (i.e. to the cache).
    // The pointer is taken over.
    void addBucket (uInt rownr, ISMBucket* bucket);

    // Make the current bucket in the cache dirty (i.e. something has been
    // changed in it and it needs to be written when removed from the cache).
    // (used by ISMColumn::putValue).
    void setBucketDirty();


private:
    // Copy constructor (only meant for clone function).
    ISMBase (const ISMBase& that);

    // Assignment cannot be used.
    ISMBase& operator= (const ISMBase& that);

    // Flush the data and close the storage manager.
    // All StManColumnMirAIO objects will write their data and close
    // themselves.
    virtual void close (AipsIO&);

    // Let the storage manager create files as needed for a new table.
    // This allows a column with an indirect array to create its file.
    virtual void create (uInt nrrow);

    // Open the storage manager file for an existing table, read in
    // the data, and let the ISMColumn objects read their data.
    // The <src>AipsIO</src> argument is obsolete and will be removed in
    // the future.
    virtual void open (uInt nrrow, AipsIO&);

    // Reopen the storage manager files for read/write.
    virtual void reopenRW();

    // Let the storage manager initialize itself.
    // It is used by create and open.
    void init();

    // Add rows to the storage manager.
    // Per column it extends the interval for which the last value written
    // is valid.
    virtual void addRow (uInt nrrow);

    // Delete a row from all columns.
    virtual void removeRow (uInt rownr);

    // Do the final addition of a column.
    // The <src>DataManagerColumn</src> object has already been created
    // (by the <src>makeXXColumn</src> function) and added to
    // <src>colSet_p</src>. However, it still has to be added to the
    // data files, which is done by this function. It uses the
    // pointer to find the correct column in the <src>colSet_p</src>.
    virtual void addColumn (DataManagerColumn*);

    // Remove a column from the data file and the <src>colSet_p</src>.
    // The <src>DataManagerColumn</src> object gets deleted..
    virtual void removeColumn (DataManagerColumn*);

    // Create a column in the storage manager on behalf of a table column.
    // The caller has to delete the newly created object.
    // <group>
    // Create a scalar column.
    virtual DataManagerColumn* makeScalarColumn (const String& name,
						 int dataType,
						 const String& dataTypeID);
    // Create a direct array column.
    virtual DataManagerColumn* makeDirArrColumn (const String& name,
						 int dataType,
						 const String& dataTypeID);
    // Create an indirect array column.
    virtual DataManagerColumn* makeIndArrColumn (const String& name,
						 int dataType,
						 const String& dataTypeID);
    // </group>

    // Get the cache object.
    // This will construct the cache object if not present yet.
    // The cache object will be deleted by the destructor.
    BucketCache& getCache();

    // Get the index object.
    // This will construct the index object if not present yet.
    // The index object will be deleted by the destructor.
    ISMIndex& getIndex();

    // Construct the cache object (if not constructed yet).
    void makeCache();

    // Construct the index object (if not constructed yet).
    void makeIndex();

    // Write the index (at the end of the file).
    void writeIndex();


    //# Declare member variables.
    // Name of data manager.
    String       dataManName_p;
    // Unique nr for column in this storage manager.
    uInt         uniqnr_p;
    // The number of rows in the columns.
    uInt         nrrow_p;
    // The assembly of all columns.
    PtrBlock<ISMColumn*>  colSet_p;
    // The cache with the ISM buckets.
    BucketCache* cache_p;
    // The file containing all data.
    BucketFile*  file_p;
    // The ISM bucket index.
    ISMIndex*    index_p;
    // The persistent cache size.
    uInt persCacheSize_p;
    // The actual cache size.
    uInt cacheSize_p;
    // The bucket size.
    uInt bucketSize_p;
    // Check a positive bucketsize?
    Bool checkBucketSize_p;
    // The size of a uInt in external format (local or canonical).
    uInt uIntSize_p;
    // Have the data to be stored in canonical or local format.
    Bool asCanonical_p;
    // A temporary read/write buffer (also for other classes).
    char* tempBuffer_p;
};


inline uInt ISMBase::cacheSize() const
    { return cacheSize_p; }

inline uInt ISMBase::uniqueNr()
{
    return uniqnr_p++;
}

inline uInt ISMBase::nrow() const
{
    return nrrow_p;
}

inline uInt ISMBase::bucketSize() const
{
    return bucketSize_p;
}

inline uInt ISMBase::uIntSize() const
{
    return uIntSize_p;
}

inline Bool ISMBase::asCanonical() const
{
    return asCanonical_p;
}

inline char* ISMBase::tempBuffer() const
{
    return tempBuffer_p;
}

inline BucketCache& ISMBase::getCache()
{
    if (cache_p == 0) {
	makeCache();
    }
    return *cache_p;
}

inline ISMIndex& ISMBase::getIndex()
{
    if (index_p == 0) {
	makeIndex();
    }
    return *index_p;
}

inline ISMColumn& ISMBase::getColumn (uInt colnr)
{
    return *(colSet_p[colnr]);
}


#endif
