//# VelocityMachine.cc: Calculates between velocities and frequencies
//# Copyright (C) 1998
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

//# Includes
#include <trial/Measures/VelocityMachine.h>
#include <aips/Mathematics/Math.h>
#include <aips/Arrays/Vector.h>
#include <aips/Measures/UnitVal.h>
#include <aips/Measures/MeasFrame.h>
#include <aips/Measures/MeasConvert.h>

//# Constructors

VelocityMachine::VelocityMachine(const MFrequency::Ref &frqref, 
				 const Unit &frqun,
				 const MVFrequency &rest,
				 const MDoppler::Ref &velref, 
				 const Unit &velun) :
  fref_p(frqref), fun_p(frqun), rest_p(rest),
  vref_p(velref), vun_p(velun) {
    vfm_p = (MFrequency::Types) fref_p.getType();
    init();
}

VelocityMachine::VelocityMachine(const MFrequency::Ref &frqref, 
				 const Unit &frqun,
				 const MVFrequency &rest,
				 const MDoppler::Ref &velref, 
				 const Unit &velun,
				 const MeasFrame &frame) :
  fref_p(frqref), fun_p(frqun), rest_p(rest),
  vref_p(velref), vun_p(velun) {
    fref_p.set(frame);
    vfm_p = (MFrequency::Types) fref_p.getType();
    init();
}

VelocityMachine::VelocityMachine(const MFrequency::Ref &frqref, 
				 const Unit &frqun,
				 const MVFrequency &rest,
				  const MFrequency::Types &velframe,
				 const MDoppler::Ref &velref, 
				 const Unit &velun) :
  fref_p(frqref), fun_p(frqun), rest_p(rest), vfm_p(velframe),
  vref_p(velref), vun_p(velun) {
    init();
}

VelocityMachine::VelocityMachine(const MFrequency::Ref &frqref, 
				 const Unit &frqun,
				 const MVFrequency &rest,
				 const MFrequency::Types &velframe,
				 const MDoppler::Ref &velref, 
				 const Unit &velun,
				 const MeasFrame &frame) :
  fref_p(frqref), fun_p(frqun), rest_p(rest), vfm_p(velframe),
  vref_p(velref), vun_p(velun) {
    fref_p.set(frame);
    init();
}

VelocityMachine::VelocityMachine(const VelocityMachine &other) {
  copy(other);
  init();
}

VelocityMachine &VelocityMachine::operator=(const VelocityMachine &other) {
  if (this != &other) {
    copy(other);
    init();
  };
  return *this;
}

//# Destructor
VelocityMachine::~VelocityMachine() {}

//# Operators
const Quantum<Double> &VelocityMachine::operator()(const MVFrequency &in) {
  resv_p.setValue(cvvo_p(cvfv_p(in).
                         toDoppler(rest_p).getValue()).
                  getValue().get().getValue() / vfac_p);
  return resv_p;
}

const Quantum<Double> &VelocityMachine::operator()(const MVDoppler &in) {
  resf_p.setValue(MVFrequency(cvvf_p(MFrequency::fromDoppler(cvov_p(in),
                                                             rest_p, vfm_p).
                                     getValue()).
                              getValue().getValue()).get(fun_p).getValue());
  return resf_p;
}

const Quantum<Double> &VelocityMachine::operator()(const Quantity &in) {
  static UnitVal Velocity = UnitVal::LENGTH/UnitVal::TIME;
  if (in.getFullUnit().getValue() == Velocity ||
      in.getFullUnit().getValue() == UnitVal::NODIM) {
    return this->operator()(MVDoppler(in));
  };
  return this->operator()(MVFrequency(in));
}

const Quantum<Double> &VelocityMachine::makeVelocity(Double in) {
  resv_p.setValue(cvvo_p(cvfv_p(in).
			 toDoppler(rest_p).getValue()).
		  getValue().get().getValue() / vfac_p);
  return resv_p;
}

const Quantum<Double> &VelocityMachine::makeFrequency(Double in) {
  resf_p.setValue(MVFrequency(cvvf_p(MFrequency::fromDoppler(cvov_p(in),
							     rest_p, vfm_p).
				     getValue()).
			      getValue().getValue()).get(fun_p).getValue());
  return resf_p;
}

const Quantum<Vector<Double> > &VelocityMachine::
makeVelocity(const Vector<Double> &in) {
  uInt n = in.nelements();
  vresv_p.getValue().resize(n);
  for (uInt i=0; i<n; i++) {
    vresv_p.getValue()(i) = cvvo_p(cvfv_p(in(i)).
				   toDoppler(rest_p).getValue()).
      getValue().get().getValue() / vfac_p;
  };
  return vresv_p;
}

const Quantum<Vector<Double> > &VelocityMachine::
makeFrequency(const Vector<Double> &in) {
  uInt n = in.nelements();
  vresf_p.getValue().resize(n);
  for (uInt i=0; i<n; i++) {
    vresf_p.getValue()(i) = MVFrequency(cvvf_p(MFrequency::
					       fromDoppler(cvov_p(in(i)),
							   rest_p, vfm_p).
					       getValue()).
					getValue().getValue()).get(fun_p).
      getValue();
  };
  return vresf_p;
}

//# Member functions

void VelocityMachine::reCalculate() {
  init();
}

void VelocityMachine::set(const MFrequency::Ref &in) {
  fref_p = in;
  init();
}

void VelocityMachine::set(const Unit &in) {
  static UnitVal Velocity = UnitVal::LENGTH/UnitVal::TIME;
  if (in.getValue() == Velocity || in.getValue() == UnitVal::NODIM) {
    vun_p = in;
  } else {
    fun_p = in;
  };
  init();
}

void VelocityMachine::set(const MVFrequency &in) {
  rest_p = in;
  init();
}

void VelocityMachine::set(const MFrequency::Types &in) {
  vfm_p = in;
  init();
}

void VelocityMachine::set(const MDoppler::Ref &in) {
  vref_p = in;
  init();
}

void VelocityMachine::set(const MeasFrame &in) {
  fref_p.set(in);
  init();
}

//# Private member functions
void VelocityMachine::init() {
  vfac_p = MVDoppler(Quantity(1, vun_p)).get().getValue();
  resv_p.setUnit(vun_p);
  resf_p.setUnit(fun_p);
  vresv_p.setUnit(vun_p);
  vresf_p.setUnit(fun_p);
  cvfv_p = MFrequency::Convert(fref_p, vfm_p);
  cvvf_p = MFrequency::Convert(vfm_p, fref_p);
  cvvo_p = MDoppler::Convert(MDoppler::BETA, vref_p);
  cvov_p = MDoppler::Convert(vref_p, MDoppler::BETA);
  cvfv_p.set(fun_p);
  cvov_p.set(vun_p);
}

void VelocityMachine::copy(const VelocityMachine &other) {
  fref_p = other.fref_p;
  fun_p = other.fun_p;
  rest_p = other.rest_p;
  vfm_p = other.vfm_p;
  vref_p = other.vref_p;
  vun_p = other.vun_p;
}
