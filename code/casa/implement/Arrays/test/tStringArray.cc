//# tStringArray.cc: This program tests Array<String> and related classes
//# Copyright (C) 1993,1994,1995,2001
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This program is free software; you can redistribute it and/or modify it
//# under the terms of the GNU General Public License as published by the Free
//# Software Foundation; either version 2 of the License, or (at your option)
//# any later version.
//#
//# This program is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
//# more details.
//#
//# You should have received a copy of the GNU General Public License along
//# with this program; if not, write to the Free Software Foundation, Inc.,
//# 675 Massachusetts Ave, Cambridge, MA 02139, USA.
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


#include <aips/iostream.h>

#include <aips/aips.h>
#include <aips/Arrays/Array.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/Matrix.h>
#include <aips/Arrays/Cube.h>
#include <aips/Arrays/ArrayError.h>
#include <aips/BasicMath/Math.h>
#include <aips/Utilities/Assert.h>
#include <aips/BasicSL/String.h>

int main()
{
    Vector<String> vs(5);
    vs(0) = "Every";vs(1) = "Good";vs(2) = "Boy";vs(3) = "Deserves";
    vs(4) = "Fudge";
    AlwaysAssertExit(vs(0) == "Every");
    AlwaysAssertExit(vs(1) == "Good");
    AlwaysAssertExit(vs(2) == "Boy");
    AlwaysAssertExit(vs(3) == "Deserves");
    AlwaysAssertExit(vs(4) == "Fudge");

    cout << "OK\n";
    return 0;

}
