//# TSMFile.h: File object for Tiled Storage Manager
//# Copyright (C) 1995,1996,1997
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

#if !defined(AIPS_TSMFILE_H)
#define AIPS_TSMFILE_H

#if defined(_AIX)
#pragma implementation ("TSMFile.cc")
#endif 

//# Includes
#include <aips/aips.h>
#include <aips/Tables/BucketFile.h>

//# Forward Declarations
class TiledStMan;
class AipsIO;


// <summary>
// File object for Tiled Storage Manager.
// </summary>

// <use visibility=local>

// <reviewed reviewer="" date="" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> <linkto class=TiledStMan>TiledStMan</linkto>
// </prerequisite>

// <etymology>
// TSMFile represents a data file for the Tiled Storage Manager.
// </etymology>

// <synopsis>
// A TSMFile object represents a data file. Currently it is meant
// for the TiledStMan classes, but it can easily be turned into
// a more general storage manager file class.
// <br>
// Creation of a TSMFile object does not open the file.
// An explicit open call has to be given before the file can be used.
// <p>
// Underneath it uses a BucketFile to access the file.
// In this way the IO details are well encapsulated.
// </synopsis> 

// <motivation>
// Encapsulate the Tiled Storage Manager file details.
// </motivation>

//# <todo asof="$DATE:$">
//# </todo>


class TSMFile
{
public:
    // Create a TSMFile object (with corresponding file).
    // The sequence number gets part of the file name.
    TSMFile (TiledStMan* stMan, uInt fileSequenceNr);

    // Read the object back.
    // The file is not opened until the first access,
    // thus until the file descriptor is asked for the first time.
    // It checks if the sequence number matches the expected one.
    TSMFile (TiledStMan* stMan, AipsIO& ios, uInt seqnr);

    // The destructor closes the file.
    ~TSMFile();

    // Write the object.
    void putObject (AipsIO& ios) const;

    // Get the object.
    void getObject (AipsIO& ios);

    // Open the file if not open yet.
    void open();

    // Return the BucketFile object (to be used in the BucketCache).
    BucketFile* bucketFile();

    // Return the logical file length.
    uInt length() const;

    // Return the file sequence number.
    uInt sequenceNumber() const;

    // Increment the logical file length.
    void extend (uInt increment);


private:
    // The parent TiledStMan object.
    TiledStMan* stmanPtr_p;
    // The file sequence number.
    uInt fileSeqnr_p;
    // The file object.
    BucketFile* file_p;
    // The (logical) length of the file.
    uInt length_p;
	    

    // Forbid copy constructor.
    TSMFile (const TSMFile&);

    // Forbid assignment.
    TSMFile& operator= (const TSMFile&);
};


inline uInt TSMFile::length() const
    { return length_p; }

inline uInt TSMFile::sequenceNumber() const
    { return fileSeqnr_p; }

inline void TSMFile::extend (uInt increment)
    { length_p += increment; }

inline BucketFile* TSMFile::bucketFile()
    { return file_p; }

inline void TSMFile::open()
    { file_p->open(); }



#endif
