//# ConvolveGridder.h: Definition for Convolutional Gridder
//# Copyright (C) 1996,1997,1999
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#if !defined(AIPS_CONVOLVEGRIDDER_H)
#define AIPS_CONVOLVEGRIDDER_H

#include <trial/Mathematics/Gridder.h>

// <summary>
// Does convolutional gridding
// </summary>

template <class Domain, class Range>
class ConvolveGridder : public Gridder<Domain, Range>{
public:

  ConvolveGridder(const IPosition& shape, const Vector<Domain>& scale,
		  const Vector<Domain>& offset, const String& convType="SF");

  virtual void setConvolutionFunction(const String& type);

  virtual ~ConvolveGridder() {};

  virtual Bool grid(Array<Range>& gridded,
		    const Vector<Domain>& position,
		    const Range& value);

  virtual Bool degrid(const Array<Range>& gridded,
		      const Vector<Domain>& position,
		      Range& value);

  Vector<Double>& cFunction();

  Vector<Int>& cSupport();

  Int& cSampling();

protected:

  virtual Range correctionFactor1D(Int loc, Int len);

private:
  Vector<Double> convFunc;
  Vector<Int> supportVec;
  Vector<Int> loc;
  Int sampling;
  Int support;
  String cType;
};
#endif
