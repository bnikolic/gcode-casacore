//# tTableDescHyper.cc: Test program for hypercolumns in TableDesc
//# Copyright (C) 1994,1995,1996
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

#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ScaColDesc.h>
#include <aips/Tables/ArrColDesc.h>
#include <aips/Arrays/Vector.h>
#include <aips/Arrays/ArrayUtil.h>
#include <aips/Arrays/ArrayIO.h>
#include <aips/Utilities/String.h>
#include <aips/Exceptions/Error.h>
#include <iostream.h>

// <summary> Test program for hypercolumns in TableDesc </summary>

// This program tests the hypercolumn functionality in class TableDesc.
// The results are written to stdout. The script executing this program,
// compares the results with the reference output file.

void a();
void b();
void excpDesc();

main (int argc) {
    try {
	a();
	b();
	if (argc < 2) {
	    excpDesc();
	}
    } catch (AipsError x) {
	cout << "Caught an exception: " << x.getMesg() << endl;
	return 1;
    } end_try;
    return 0;                           // exit with success status
}

// First build a description.
void a()
{
    // Build the table description.
    TableDesc td ("tTableDescHyper_tmp", "1", TableDesc::New);
    td.addColumn (ScalarColumnDesc<float> ("Time"));
    td.addColumn (ScalarColumnDesc<double> ("Baseline"));
    td.addColumn (ScalarColumnDesc<Complex> ("DataScalar"));
    td.addColumn (ArrayColumnDesc<float>  ("Pol", IPosition(1,16),
					   ColumnDesc::FixedShape));
    td.addColumn (ArrayColumnDesc<float>  ("Freq", 1));
    td.addColumn (ScalarColumnDesc<String> ("Id"));
    td.addColumn (ArrayColumnDesc<float>  ("Data", 2));
    td.addColumn (ArrayColumnDesc<float>  ("Weight", IPosition(2,16,25),
					   ColumnDesc::FixedShape));
    td.defineHypercolumn ("TSMExample1",
			  4,
			  stringToVector ("Data,Weight"),
			  stringToVector ("Pol,Freq,Baseline,Time"),
			  stringToVector ("Id"));
    td.defineHypercolumn ("TSMExample2",
			  4,
			  stringToVector ("Data"));
    td.defineHypercolumn ("TSMExample3",
			  3,
			  stringToVector ("Data,Weight"),
			  stringToVector ("Pol,Freq,Time"));
    td.defineHypercolumn ("TSMExample4",
			  2,
			  stringToVector ("DataScalar"),
			  stringToVector ("Baseline,Time"));
}

void b()
{
    TableDesc td ("tTableDescHyper_tmp");
    Vector<String> idNames;
    Vector<String> coordNames;
    Vector<String> dataNames;
    uInt ndim;
    cout << td.isHypercolumn ("TSMExamplea")
	 << td.isHypercolumn ("TSMExample1")
	 << td.isHypercolumn ("TSMExample2")
	 << td.isHypercolumn ("TSMExample3")
	 << td.isHypercolumn ("TSMExample4")
	 << td.isHypercolumn ("TSMExampleb") << endl;
    cout << td.hypercolumnNames().ac() << endl;
    ndim = td.hypercolumnDesc ("TSMExample1", dataNames, coordNames, idNames);
    cout << "TSMExample1: ndim=" << ndim << endl;
    cout << "  Data=" << dataNames.ac();
    cout << "Coord=" << coordNames.ac();
    cout << "Id=" << idNames.ac() << endl;
    ndim = td.hypercolumnDesc ("TSMExample2", dataNames, coordNames, idNames);
    cout << "TSMExample2: ndim=" << ndim << endl;
    cout << "  Data=" << dataNames.ac();
    cout << "Coord=" << coordNames.ac();
    cout << "Id=" << idNames.ac() << endl;
    ndim = td.hypercolumnDesc ("TSMExample3", dataNames, coordNames, idNames);
    cout << "TSMExample3: ndim=" << ndim << endl;
    cout << "  Data=" << dataNames.ac();
    cout << "Coord=" << coordNames.ac();
    cout << "Id=" << idNames.ac() << endl;
    ndim = td.hypercolumnDesc ("TSMExample4", dataNames, coordNames, idNames);
    cout << "TSMExample4: ndim=" << ndim << endl;
    cout << "  Data=" << dataNames.ac();
    cout << "Coord=" << coordNames.ac();
    cout << "Id=" << idNames.ac() << endl;
}

void excpDesc()
{
    // Build the table description.
    TableDesc td ("", "1", TableDesc::Scratch);
    td.addColumn (ScalarColumnDesc<float> ("Time"));
    td.addColumn (ScalarColumnDesc<Bool> ("TimeNotNum"));
    td.addColumn (ScalarColumnDesc<Short> ("TimeShort"));
    td.addColumn (ScalarColumnDesc<float> ("Baseline"));
    td.addColumn (ArrayColumnDesc<float>  ("Pol", 1));
    td.addColumn (ArrayColumnDesc<float>  ("Freq", 1));
    td.addColumn (ScalarColumnDesc<float> ("Id"));
    td.addColumn (ArrayColumnDesc<float>  ("Data", 2));
    td.addColumn (ArrayColumnDesc<float>  ("Data0"));
    td.addColumn (ArrayColumnDesc<float>  ("Data1", 1));
    td.addColumn (ArrayColumnDesc<float>  ("Weight", 2));
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector (""));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // no data columns
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      0,
			      stringToVector ("Data,Weight"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // ndim < 1
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // ndim != #coord
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Timex"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Timex does not exist
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight,Datax"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Datax does not exist
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Time"),
			      stringToVector ("Idx"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Idx does not exist
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,TimeNotNum"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // TimeNotNum not numeric
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,TimeShort"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Coord short not supported
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Data"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Coord Data is array > 1-dim
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Pol"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // coord vectors not at start
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data0,Weight"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Data #dim undefined
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data1,Weight"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Data #dim != Weight #ndim
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data1"),
			      stringToVector ("Pol,Freq,Baseline,Time"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Data #dim != #coordVector
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Time"),
			      stringToVector ("TimeShort"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Id short not supported
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Time"),
			      stringToVector ("Data"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Id array not supported
    } end_try;
    try {
	td.defineHypercolumn ("TSMExample",
			      4,
			      stringToVector ("Data,Weight"),
			      stringToVector ("Pol,Freq,Baseline,Time"),
			      stringToVector ("Time"));
    } catch (AipsError x) {
	cout << x.getMesg() << endl;             // Time double used 
    } end_try;
}
