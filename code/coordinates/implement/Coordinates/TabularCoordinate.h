//# TabularCoordinate.h: Table lookup 1-D coordinate, with interpolation
//# Copyright (C) 1997,1998,1999,2000,2001,2003
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
//#
//# $Id$

#if !defined(AIPS_TABULAR_COORDINATE_H)
#define AIPS_TABULAR_COORDINATE_H

#include <aips/aips.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/Vector.h>
#include <trial/Coordinates/Coordinate.h>

template<class Domain, class Range> class Interpolate1D;
template<class T> class Quantum;
class LogIO;

// <summary>
// Table lookup 1-D coordinate, with interpolation.
// </summary>

// <use visibility=export>

// <reviewed reviewer="Peter Barnes" date="1999/12/24" tests="tTabularCoordinate">
// </reviewed>

// <prerequisite>
//   <li> <linkto class=Coordinate>Coordinate</linkto>
// </prerequisite>
//
// <synopsis>
// This class is used where the world and pixel values are determined by a
// lookup table. For fractional pixel values, a linear interpolation is used. 
// The values returned for, e.g., the increment, are based on 
// the average of the whole table.  At present,
// the values must either increase or decrease monotonically.
// </synopsis>
//
// <note role=caution>
// All pixels coordinates are zero relative.
// </note>
//
// <example>
// Let's make a non-linear TabularCoordinate  and convert a pixel
// value to world (which will use linear interpolation)
// <srcblock>
//    Vector<Double> pixelValues(3); 
//    Vector<Double> worldValues(3); 
//    pixelValues(0) = 122.0;
//    pixelValues(1) = 300.0;
//    pixelValues(2) = 6524.0;
//    worldValues(0) = 1.1e6;
//    worldValues(1) = 2.1e6;
//    worldValues(2) = 2.2e6;
//
//    String unit("km");
//    String axisName("length");
//
//    TabularCoordinate tc(pixelValues, worldValues, unit, axisName);
//
//    Double world, pixel;
//    pixel = 200.12;
//    if (!tc.toWorld(world, pixel)) {
//      cerr << "Error : " << tc.errorMessage() << endl;
//    } else {
//      cerr << "pixel, world = " << pixel << ", " << world << endl;
//    }
// </srcblock>
// </example>
//
// <motivation>
// This class was motivated by the need for an irregular axis, such as a collection
// of frequencies.    For example, the SpectralCoordinate class contains a TabularCoordinate.
// </motivation>
//
//
// <thrown>
//   <li>  AipsError
// </thrown>
//
// <todo asof="1997/07/12">
// <li> Allow interpolations other than linear.
// </todo>


class TabularCoordinate : public Coordinate
{
public:
    // Default constructor.  It is equivalent to 
    // TabularCoordinate(0,1,0, "", "Tabular");
    TabularCoordinate();

    // Create a linear TabularCoordinate where 
    // <src>world = refval + inc*(pixel-refpix)</src>
    TabularCoordinate(Double refval, Double inc, Double refpix,
		      const String &unit, const String &axisName);

    // Create a linear TabularCoordinate with a Quantum-based interface where 
    // <src>world = refval + inc*(pixel-refpix)</src>.  The units of the 
    // increment (<src>inc</src>) will be converted to
    // those of the reference value (<src>refVal</src>) which will
    // then serve as the units of the Coordinate.
    TabularCoordinate(const Quantum<Double>& refval, 
                      const Quantum<Double>& inc, 
                      Double refpix, const String& axisName);

    // Construct a TabularCoordinate with the specified world values. The
    // increments and related functions return the average values
    // calculated from the first and last world values. The number of pixel
    // and world values must be the same. Normally the pixel values will be
    // 0,1,2,..., but this is not required.
    //
    // A linear interpolation/extrapolation is used for channels which are not
    // supplied. The reference channel (pixel) is chosen to be 0.  The
    // frequencies must increase or decrease monotonically (otherwise the
    // toPixel lookup would not be possible).
    TabularCoordinate(const Vector<Double> &pixelValues,
		      const Vector<Double> &worldValues,
		      const String &unit, const String &axisName);

    // Construct a TabularCoordinate with the specified world values
    // via the Quantum-based interface.  All comments for the
    // previous constructor apply
    TabularCoordinate(const Vector<Double>& pixelValues,
                      const Quantum<Vector<Double> >& worldValues,
                      const String &axisName);

    // Copy constructor (copy semantics).
    TabularCoordinate(const TabularCoordinate &other);

    // Assignment (copy semantics).
    TabularCoordinate &operator=(const TabularCoordinate &other);

    // Destructor.
    virtual ~TabularCoordinate();

    // Returns Coordinate::TABULAR.
    virtual Coordinate::Type type() const;

    // Always returns the String "Tabular".
    virtual String showType() const;

    // Always returns 1.
    // <group>
    virtual uInt nPixelAxes() const;
    virtual uInt nWorldAxes() const;
    // </group>

    // Convert a pixel position to a world position or vice versa. Returns True
    // if the conversion succeeds, otherwise it returns False and method
    // errorMessage contains an error message.  The output
    // vectors are appropriately resized.
    // <group>
    virtual Bool toWorld(Vector<Double> &world, 
			 const Vector<Double> &pixel) const;
    virtual Bool toPixel(Vector<Double> &pixel, 
			 const Vector<Double> &world) const;
    Bool toWorld(Double &world, Double pixel) const;
    Bool toPixel(Double &pixel, Double world) const;
    // </group>

    // Batch up a lot of transformations. The first (most rapidly varying) axis
    // of the matrices contain the coordinates. Return the number of failures.
    // The failures array will be at least as long as the returned number of
    // failures, and contains the indicies of the failed transformations.
    // <src>errorMessage()</src> will be set to the error from the FIRST failure. 
    // If failures is longer than the return value, the value in the excess 
    // locations is undefined.
    // <group>
    virtual uInt toWorldMany(Matrix<Double> &world,
                              const Matrix<Double> &pixel,
                              Vector<Int> &failures) const;
    virtual uInt toPixelMany(Matrix<Double> &pixel,
                              const Matrix<Double> &world,
                              Vector<Int> &failures) const;
    // </group>


    // Make absolute coordinates relative and vice-versa (with
    // respect to the referencfe value).
    // Vectors must be length <src>nPixelAxes()</src> or
    // <src>nWorldAxes()</src> or memory access errors will occur
    // <group>
    virtual void makePixelRelative (Vector<Double>& pixel) const {pixel -= crpix_p;};
    virtual void makePixelAbsolute (Vector<Double>& pixel) const {pixel += crpix_p;};
    virtual void makeWorldRelative (Vector<Double>& world) const {world -= crval_p;};
    virtual void makeWorldAbsolute (Vector<Double>& world) const {world += crval_p;};
    // </group>

    // Return the requested attribute.
    // <group>
    virtual Vector<String> worldAxisNames() const;
    virtual Vector<Double> referencePixel() const;
    virtual Matrix<Double> linearTransform() const;
    virtual Vector<Double> increment() const;
    virtual Vector<Double> referenceValue() const;
    // </group>

    // Set the value of the requested attribute.  Note that these just
    // change the internal values, they do not cause any recomputation.
    // <group>
    virtual Bool setWorldAxisNames(const Vector<String> &names);
    virtual Bool setReferencePixel(const Vector<Double> &refPix);
    virtual Bool setLinearTransform(const Matrix<Double> &xform);
    virtual Bool setIncrement(const Vector<Double> &inc) ;
    virtual Bool setReferenceValue(const Vector<Double> &refval);
    // </group>

    // Set/get the axis unit. Adjust the increment and
    // reference value by the ratio of the old and new units.
    // The unit must be compatible with the current units.
    // <group>
    virtual Bool setWorldAxisUnits(const Vector<String> &units);
    virtual Vector<String> worldAxisUnits() const;
    // </group>

    // Overwrite the world axis units with no compatibility
    // checks or adjustment.
    Bool overwriteWorldAxisUnits(const Vector<String> &units);

    // Set the world min and max ranges, for use in function <src>toMix</src>,
    // for  a lattice of the given shape (for this coordinate).
    // The implementation here gives world coordinates dangling 25% off the
    // edges of the image.       
    // The output vectors are resized.  Returns False if fails (and
    // then <src>setDefaultWorldMixRanges</src> generates the ranges)
    // with a reason in <src>errorMessage()</src>.
    // The <src>setDefaultWorldMixRanges</src> function
    // gives you [-1e99->1e99]. 
    // <group>
    virtual Bool setWorldMixRanges (const IPosition& shape);
    virtual void setDefaultWorldMixRanges ();
    virtual Vector<Double> worldMixMin () const {return worldMin_p;};
    virtual Vector<Double> worldMixMax () const {return worldMax_p;};
    //</group>

    // Set and recover the preferred world axis units.  These can be used to specify
    // a favoured unit for conversions for example.  The given units must be empty
    // or dimensionally consistent with the native world axis units, else
    // False is returned and <src>errorMessage()</src>
    // has an error message for you.  The preferred units are empty strings
    // until you explicitly set them.  The only functions in the Coordinates classes
    // which uses the preferred unit are <src>format, save, and restore</src>.
    // <group>
    virtual Bool setPreferredWorldAxisUnits (const Vector<String>& units);
    virtual Vector<String> preferredWorldAxisUnits() const;
    // </group>

    // Get the table, i.e. the pixel and world values. The length of these
    // Vectors will be zero if this axis is pure linear.
    // <group>
    Vector<Double> pixelValues() const;
    Vector<Double> worldValues() const;
    // </group>

    // Comparison function. Any private Double data members are compared    
    // with the specified fractional tolerance.  Don't compare on the specified     
    // axes in the Coordinate.  If the comparison returns False, method
    // errorMessage() contains a message about why.
    // <group>
    virtual Bool near(const Coordinate& other, 
                      Double tol=1e-6) const;
    virtual Bool near(const Coordinate& other, 
                      const Vector<Int>& excludeAxes,
                      Double tol=1e-6) const;
    // </group>

    // Find the Coordinate for when we Fourier Transform ourselves.  This pointer
    // must be deleted by the caller. Axes specifies which axes of the Coordinate
    // you wish to transform.   Shape specifies the shape of the image
    // associated with all the axes of the Coordinate.   Currently the
    // output reference pixel is always shape/2.
    virtual Coordinate* makeFourierCoordinate (const Vector<Bool>& axes,
                                               const Vector<Int>& shape) const;

    // Save the TabularCoordinate into the supplied record using the supplied field name.
    // The field must not exist, otherwise <src>False</src> is returned.
    virtual Bool save(RecordInterface &container, const String &fieldName) const;

    // Recover the TabularCoordinate from a record.
    // A null pointer means that the restoration did not succeed - probably 
    // because fieldName doesn't exist or doesn't contain a CoordinateSystem.
    static TabularCoordinate *restore(const RecordInterface &container,
				   const String &fieldName);

    // Make a copy of the TabularCoordinate using new. The caller is responsible for calling
    // delete.
    virtual Coordinate *clone() const;

private:
    Double crval_p, cdelt_p, crpix_p;
    Double matrix_p;
    String unit_p;
    String prefUnit_p;
    String name_p;
    Vector<Double> worldMin_p, worldMax_p;

    // Channel_True = channel_corrections_p(Channel_average).
    // <group>
    Interpolate1D<Double,Double> *channel_corrector_p;
    Interpolate1D<Double,Double> *channel_corrector_rev_p;
    // </group>

    // Common for assignment operator and destructor.
    void clear_self();

    // Common code for copy ctor and assignment operator.
    void copy(const TabularCoordinate &other);

    void makeNonLinearTabularCoordinate(const Vector<Double> &pixelValues,
                                        const Vector<Double> &worldValues);
};


#endif
