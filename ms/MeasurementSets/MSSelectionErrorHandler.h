//# MSSelectionErrorHandler.h: MSSelection error handler class
//# Copyright (C) 1994,1995,1996,1997,1999,2000
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

#ifndef MS_MSSELECTIONERRORHANDLER_H
#define MS_MSSELECTIONERRORHANDLER_H

//# Includes
#include <casa/aips.h>
#include <casa/Logging/LogIO.h>
#include <casa/Exceptions/Error.h>
#include <ms/MeasurementSets/MSSelectionError.h>
#include <vector>
using namespace std;

namespace casa { //# NAMESPACE CASA - BEGIN

  //# This header file defines the error handler class for the
  //# MSSelection module


  // <summary>
  // </summary>
  // <use visibility=export>
  // <reviewed reviewer="UNKNOWN" date="" tests="">
  // </reviewed>

  // <synopsis> 
  //
  // The top-level generic MSSelection error handler class.  The
  // handleError() overloadable method takes the action of reporting
  // the error.  The handleError() method of this defualt handler
  // constructs a message string and throws an exception of the type
  // supplied.  This operation has been factored out into this object
  // to allow more control on the error handler mechanism from
  // outside.
  //
  //</synopsis>
  
  class MSSelectionErrorHandler 
  {
  public:
    // The default constructor generates the message "Table error".
    MSSelectionErrorHandler();
    virtual ~MSSelectionErrorHandler ();
    
    virtual void reportError(const char *token,const String source=String(""));
    virtual String constructMessage();

    virtual void reset();
    virtual void handleError(MSSelectionError&);
  protected:
    vector<String> tokenList, messageList;
  };

  // <synopsis> 
  //
  // The handleError() method is overloaded to send the accumulated
  // error messages to the LogIO object as warning messages.
  //
  //</synopsis>
  
  class MSSelectionLogError: public MSSelectionErrorHandler
  {
  public:
    MSSelectionLogError():MSSelectionErrorHandler() {}
    virtual ~MSSelectionLogError() {}
    virtual void handleError(MSSelectionError& mssErrorType)
    {
      if (messageList.size() > 0)
	{
	  String mesg(constructMessage());
	  mssErrorType.addMessage(mesg);
	  LogIO logIO;
	  logIO << mssErrorType.getMesg() << LogIO::WARN;
      }
    }
  };
} //# NAMESPACE CASA - END


#endif
