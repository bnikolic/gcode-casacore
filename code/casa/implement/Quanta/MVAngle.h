//# MVAngle.h: Class to handle angle type conversions and I/O
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

#if !defined(AIPS_MVANGLE_H)
#define AIPS_MVANGLE_H

#if defined(_AIX)
#pragma implementation ("MVAngle.cc")
#endif

//# Includes
#include <aips/aips.h>
#include <aips/Measures/Quantum.h>

//# Forward Declarations
imported class ostream;
class String;
class MUString;

//# Constants (SUN compiler does not accept non-simple default arguments)

// <summary>
// Class to handle angle type conversions and I/O
// </summary>

// <use visibility=export>

// <reviewed reviewer="" date="" tests="tMeasure" demos="">
// </reviewed>

// <prerequisite>
// <li> <linkto class=Quantum>Quantum</linkto>
// </prerequisite>
//
// <etymology>
// From Measure, Value and Angle
// </etymology>
//
// <synopsis>
// An MVAngle is a simple Double, to be used for angle conversions and I/O.
// It can be constructed from a Double (in which case radians are assumed),
// or from a Quantity (<src>Quantum<Double></src>). Quantities must be in
// either angle or time units.<br>
// It has an automatic conversion to Double, so all standard mathematical
// operations can operate on it.<br>
// The class has a number of special member operations:
// <ul>
//  <li> <src>MVAngle operator()</src> will normalise the angle between
//	-180 and +180(inclusive) degrees and return the value 
//  <li> <src>MVAngle operator(Double)</src> will normalise the angle 
//	using the value specified (and return the value)
//	in fractions of a circle (this was chosen rather than radians to make
// 	for easier and more precise programming) as a lower bound. I.e.
//	(-0.5) will normalise between -180 and +180 degrees, (0.) between
//	0 and 360 degrees, (-0.25) between -90 and +270 degrees,
//	(5.) between 1800 and 2160 dgrees.
//   <li> <src>MVAngle operator(MVAngle)</src> will normalise (and
//	return the normalised value) to within 180 degrees of the
//	argument value. This is useful for making a range of angles
//	contiguous.
//   <li> <src>Double radian()</src> will return value in radians
//   <li> <src>Double degree()</src> will return value in degrees
//   <li> <src>Double circle()</src> will return value in fraction of circles
//   <li> <src>MVAngle coAngle()</src> will return 90-angle (or rather 
//		pi/2 - angle), with (0) normalisation.
//   <li> <src>Quantity get()</src> will return radians
//   <li> <src>Quantity get(Unit)</src> will return in specified units 
//		(angle or time)
// </ul>
// Output formatting is done with the <src><<</src> statement, with the
// following rules:
// <ul>
//   <li> standard output is done in the following format:
//	<src>+ddd.mm.ss.tt</src> with a floating sign. The number of
//	digits presented will be based on the precision attached to the
//	current stream
//   <li> output can be formatted by using either the <src>setFormat()</src>
//	method for global angle format setting, or the output of
//	<src>MVAngle::Format()</src> data for a once off change (see later).
//	Formats have a first argument which
//	determines the type (default, if not given, MVAngle::ANGLE, other
//	possibility MVAngle::TIME (as hh:mm:ss.tt..),
//	the second the number of digits wanted (default stream precision),
//	with a value:
//	<ul>
//	  <li> <3 : ddd.. only
//	  <li> <5 : ddd.mm.
//	  <li> <7 : ddd.mm.ss
//	  <li> >6 : with precision-6 t's added
//	</ul> 
//	comparable for time. <note> The added periods are to enable input
//	checking of the format. Look at the 'clean' types to bypass them.
//	</note>
//	The output format can be modified with modifiers (specify as
//	MVAngle::ANGLE | MVAngle::MOD (or + MVAngle::MOD)). 
//	<note role=caution>
//	 For overloading/casting problems with some compilers, the
//	use of modifiers necessitates either the presence of a precision
//	(i.e. <src>(A|B, prec)</src>), or an explicit cast:
//	<src>((MVAngle::formatTypes)(A|B))</src>, or make use of
//	the provided <src>ANGLE[_CLEAN][_NO_D[M]]</src> and
//	<src>TIME[_CLEAN][_NO_H[M]].</src>
//	</note>
//	The modifiers
//	can be:
//	<ul>
//	 <li> <src>MVAngle::CLEAN</src> to suppress leading or trailing
//		periods (or colons for TIME). + and leading zeroes in degree
//		field for angle representation will be replaced with a space.
//		Note that he result can not be
//		read automatically.
//	 <li> <src>MVAngle::NO_D</src> (or <src>NO_H</src>) to suppress
//		the output of degrees (or hours): useful for offsets
//	 <li> <src>MVAngle::NO_DM</src> (or <src>NO_HM</src>), to
//		suppress the degrees and minutes.
//	 <li> <src>MVAngle::DIG2</src> to allow only 2 digits for degrees
//	</ul>
//	Output in formats like <src>20'</src> can be done via the standard
//	Quantum output (e.g. <src> stream << angle.get("'") </src>).
//   <li> Available formats:
//	<ul>
//	  <li> MVAngle::ANGLE in +ddd.mm.ss.ttt format
//	  <li> MVAngle::TIME in hh:mm:ss.ttt format
//	  <li> MVAngle::[ANGLE|TIME]_CLEAN format without superfluous periods
//	  <li> MVAngle::[ANGLE|TIME][_CLEAN]_NO_[D|H][M] in format with
//		leading zero fields left empty.
//	  <li> MVAngle::CLEAN modifier for suppressing superfluous periods
//	  <li> MVAngle::NO_[D|H][M] modifier to suppress first field(s)
//	  <li> MVAngle::DIG2 modifier to output in +dd.mm.ss.ttt format
//	</ul>
// </ul>
// The default formatting can be overwritten by a 
// <src> MVAngle::setFormat(); </src> statement; which returns an 
// MVAngle::Format
// structure, that can be used in a subsequent one to reset to previous.
// The format set holds for all MVAngle output on all streams.<br>
// Temporary formats (i.e. for one MVAngle output only), can be set by
// outputting a format (i.e. <src> stream << MVAngle::Format() << ... </src>).
// <note> A setFormat() will also reset any lingering temporary format.
// A setFormat(getFormat()) will reset without changing. Problems could
// arise in parallel processors. </note>
// Input can be read if the values are in any of the above (non-clean) output
// formats. <br>
// For other formatting practice, the output can be written to a String with
// the string() member function.<br>
// Strings and input can be converted to an MVAngle (or Quantity) by
// <src>Bool read(Quantity &out, const String &in)</src> and
// <src> istream >> MVAngle &</src>. In the latter case the actual
// reading is done by the String read, which reads between white-spaces.<br>
// The following input formats (note no blanks allowed) are supported
// (+stands for an optional + or -; v for an unsigned integer; dv for a
// floating number. [] indicate optional values. Separating codes are
// case insensitive):
// <ul>
//   <li> +[v].[v].[dv]    -- value in deg, arcmin, arcsec
//   <li> +[v]D[v[M[dv]]]  -- value in deg, arcmin, arcsec
//   <li> +[v]:[v[:[dv]]]  -- value in h, min, s
//   <li> +[v]H[v[M[dv]]]  -- value in h, min, s
//   <li> +[v]{D|H|:}[dv]  -- value in deg (or h), arcmin (or min)
//   <li> +dv[unit string] -- value in time or angle units. rad default
// </ul>
// Examples of valid strings:
// <srcblock>
//	5::2.59		5h + 0min + 2.59 s
//	5..2.59		5deg + 0arcmin + 2.59arcsec
//	5.259		5.259 rad
//	5..259		5deg + 259arcsec
//	5.259a		5.259 * pi * 2 *365.25 rad (normalised)
// </srcblock>
// </synopsis>
//
// <example>
// See synopsis
// </example>
//
// <motivation>
// To be able to format angle-like values in user-required ways.
// </motivation>
//
// <todo asof="1996/11/15">
//   <li> Nothing I know of
// </todo>

class MVAngle {

    public:

//# Enumerations
// Format types
    enum formatTypes {
	ANGLE,
	TIME,
	CLEAN 			= 4,
	NO_D 			= 8,
	NO_DM 			= NO_D+16,
	DIG2			= 1024,
	NO_H 			= NO_D,
	NO_HM 			= NO_DM,
	ANGLE_CLEAN 		= ANGLE + CLEAN,
	ANGLE_NO_D 		= ANGLE + NO_D,
	ANGLE_NO_DM 		= ANGLE + NO_DM,
	ANGLE_CLEAN_NO_D	= ANGLE + CLEAN + NO_D,
	ANGLE_CLEAN_NO_DM	= ANGLE + CLEAN + NO_DM,
	TIME_CLEAN 		= TIME + CLEAN,
	TIME_NO_H 		= TIME + NO_H,
	TIME_NO_HM 		= TIME + NO_HM,
	TIME_CLEAN_NO_H		= TIME + CLEAN + NO_H,
	TIME_CLEAN_NO_HM	= TIME + CLEAN + NO_HM ,
	MOD_MASK		= NO_DM + CLEAN + DIG2};

//# Local structure
// Format structure
    class Format {
	public:
	friend class MVAngle;
	Format(MVAngle::formatTypes intyp = MVAngle::ANGLE,
	       uInt inprec = 0) :
	typ(intyp), prec(inprec) {;};
	Format(uInt inprec) :
	typ(MVAngle::ANGLE), prec(inprec) {;};
// Construct from type and precision (present due to overlaoding problems)
	Format(uInt intyp, uInt inprec) :
	typ((MVAngle::formatTypes) intyp), prec(inprec) {;};
	Format(const Format &other) :
	typ(other.typ), prec(other.prec) {;};
	private:
	MVAngle::formatTypes typ;
	uInt prec;
    };

//# Friends
// Output an angle
    friend ostream &operator<<(ostream &os, const MVAngle &meas);
// Input an angle
    friend istream &operator>>(istream &is, MVAngle &meas);
// Set a temporary format
    friend ostream &operator<<(ostream &os, const MVAngle::Format &form);

//# Constructors
// Default constructor: generate a zero value
    MVAngle();
// Copy constructor
    MVAngle(const MVAngle &other);
// Copy assignment
    MVAngle &operator=(const MVAngle &other);
// Constructor from Double
    explicit MVAngle(Double d);
// Constructor from Quantum : value can be an angle or time
// <thrown>
//   <li> AipsError if not a time or angle
// </thrown>
    MVAngle(const Quantity &other);

// Destructor
    ~MVAngle();

//# Operators
// Conversion operator
    operator Double() const;
// Normalisation between -180 and +180 degrees (-pi and +pi)
    const MVAngle &operator()();
// Normalisation between 2pi*norm and 2pi*norm + 2pi
    const MVAngle &operator()(Double norm);
// Normalisation between norm-pi and norm+pi
    const MVAngle &operator()(const MVAngle &norm);

//# General member functions
  // Check if String unit
  static Bool unitString(UnitVal &uv, String &us, MUString &in);
  // Make res angle Quantity from string in angle/time-like format. In the
  // case of String input, also quantities are recognised.
  // <group> 
  static Bool read(Quantity &res, const String &in);
  static Bool read(Quantity &res, MUString &in);
  // </group>
// Make co-angle (e.g. zenith distance from elevation)
    MVAngle coAngle() const;
// Get value in given unit
// <group>
    Double radian() const;
    Double degree() const;
    Double circle() const;
    Quantity get() const;
    Quantity get(const Unit &inunit) const;
// </group>
// Output data
// <group>
    String string() const;
    String string(MVAngle::formatTypes intyp, uInt inprec = 0) const;
    String string(uInt intyp, uInt inprec) const;
    String string(uInt inprec) const;
    String string(const MVAngle::Format &form) const;
    void print(ostream &oss, const MVAngle::Format &form) const;
// </group>
// Set default format
// <group>
    static Format setFormat(MVAngle::formatTypes intyp, 
			    uInt inprec = 0);
    static Format setFormat(uInt intyp, uInt inprec);
    static Format setFormat(uInt inprec = 0);
    static Format setFormat(const Format &form);
// </group>
// Get default format
    static Format getFormat();
// Get code belonging to string. 0 if not known
     static MVAngle::formatTypes  giveMe(const String &in);

    private:
//# Data
// Value
    Double val;
// Default format
    static MVAngle::Format defaultFormat;
// Temporary format
// <group>
    static MVAngle::Format interimFormat;
    static Bool interimSet;
// </group>

//# Member functions
};

// Global functions
// Output/Input
// <group name=output>
ostream &operator<<(ostream &os, const MVAngle &meas);
ostream &operator>>(ostream &is, MVAngle &meas);
ostream &operator<<(ostream &os, const MVAngle::Format &form);
// </group>

#endif
