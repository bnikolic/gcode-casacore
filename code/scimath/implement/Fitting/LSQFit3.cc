//# LSQFit3.cc: Basic class for leastr squares fitting -- Record from/to
//# Copyright (C) 2005
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
#include <scimath/Fitting/LSQFit.h>
#include <casa/Arrays/IPosition.h>
#include <casa/Arrays/Vector.h>
#include <casa/Containers/Record.h>
#include <casa/Containers/RecordFieldId.h>
#include <casa/Containers/RecordInterface.h>

namespace casa { //# NAMESPACE CASA - BEGIN

  // Constants
  const String LSQFit::recid     = String("recid");
  const String LSQFit::state     = String("state");
  const String LSQFit::nun       = String("nun");
  const String LSQFit::ncon      = String("ncon");
  const String LSQFit::prec      = String("prec");
  const String LSQFit::startnon  = String("startnon");
  const String LSQFit::nonlin    = String("nonlin");
  const String LSQFit::rank      = String("rank");
  const String LSQFit::nnc       = String("nnc");
  const String LSQFit::piv       = String("piv");
  const String LSQFit::constr    = String("constr");
  const String LSQFit::known     = String("known");
  const String LSQFit::errors    = String("error");
  const String LSQFit::sol       = String("sol");
  const String LSQFit::lar       = String("lar");
  const String LSQFit::wsol      = String("wsol");
  const String LSQFit::wcov      = String("wcov");
  const String LSQFit::nar       = String("nar");

  Bool LSQFit::toRecord(String &error, RecordInterface &out) const {
    out.define(RecordFieldId(recid),     ident());
    out.define(RecordFieldId(state),     static_cast<Int>(state_p));
    out.define(RecordFieldId(nun),       static_cast<Int>(nun_p));
    out.define(RecordFieldId(ncon),      static_cast<Int>(ncon_p));
    out.define(RecordFieldId(prec),      prec_p);
    out.define(RecordFieldId(startnon),  startnon_p);
    out.define(RecordFieldId(nonlin),    nonlin_p);
    out.define(RecordFieldId(rank),      static_cast<Int>(r_p));
    out.define(RecordFieldId(nnc),       static_cast<Int>(nnc_p));
    if (!norm_p->toRecord(error, out)) return False;
    if (piv_p    && !LSQMatrix::putCArray(error, out, piv, n_p,
					  piv_p)) return False;
    if (constr_p && !LSQMatrix::putCArray(error, out, constr, n_p*ncon_p,
					  constr_p)) return False; 
    if (known_p  && !LSQMatrix::putCArray(error, out, known, n_p,
					  known_p)) return False;
    if (error_p  && !LSQMatrix::putCArray(error, out, errors, N_ErrorField,
					  error_p)) return False; 
    if (nceq_p   && !nceq_p->toRecord(error, out)) return False;
    if (sol_p    && !LSQMatrix::putCArray(error, out, sol, n_p,
					  sol_p)) return False; 
    if (lar_p    && !LSQMatrix::putCArray(error, out, lar, n_p*n_p,
					  lar_p)) return False; 
    if (wsol_p   && !LSQMatrix::putCArray(error, out, wsol, n_p,
					  wsol_p)) return False; 
    if (wcov_p   && !LSQMatrix::putCArray(error, out, wcov, n_p*n_p,
					  wcov_p)) return False;
    if (nar_p) {
      Record rnar;
      if (!nar_p->toRecord(error, rnar)) return False;
      out.defineRecord(RecordFieldId("nar"), rnar);
    };
    return True;
  }
  
  Bool LSQFit::fromRecord(String &error, const RecordInterface &in) {
    if (in.isDefined(recid) &&
	in.type(in.idToNumber(RecordFieldId(recid))) == TpString &&
	in.isDefined(state) &&
	in.type(in.idToNumber(RecordFieldId(state))) == TpInt &&
	in.isDefined(nun) &&
	in.type(in.idToNumber(RecordFieldId(nun))) == TpInt &&
	in.isDefined(ncon) &&
	in.type(in.idToNumber(RecordFieldId(ncon))) == TpInt &&
	in.isDefined(prec) &&
	in.type(in.idToNumber(RecordFieldId(prec))) == TpDouble &&
	in.isDefined(startnon) &&
	in.type(in.idToNumber(RecordFieldId(startnon))) == TpDouble &&
	in.isDefined(nonlin) &&
	in.type(in.idToNumber(RecordFieldId(nonlin))) == TpDouble &&
	in.isDefined(rank) &&
	in.type(in.idToNumber(RecordFieldId(rank))) == TpInt &&
	in.isDefined(nnc) &&
	in.type(in.idToNumber(RecordFieldId(nnc))) == TpInt) {
      String rrecid;
      in.get(RecordFieldId(recid), rrecid);
      if (rrecid != ident()) {
	error += String("Unknown record identity ") + rrecid +
	  " for fitting record";
	return False;
      };
      uInt rnun;
      uInt rncon;
      in.get(RecordFieldId(nun), static_cast<Int>(rnun));
      in.get(RecordFieldId(ncon), static_cast<Int>(rncon));
      set(rnun, rncon);
      in.get(RecordFieldId(prec), prec_p);
      in.get(RecordFieldId(startnon), startnon_p);
      in.get(RecordFieldId(nonlin), nonlin_p);
      in.get(RecordFieldId(rank), static_cast<Int>(r_p));
      in.get(RecordFieldId(state), static_cast<Int>(state_p));
      in.get(RecordFieldId(nnc), static_cast<Int>(nnc_p));
      if (!norm_p->fromRecord(error, in)) return False;
      uInt lInt;
      if (in.isDefined(piv) &&
	  !LSQMatrix::getCArray(error, in, piv, n_p, piv_p)) return False;
      if (in.isDefined(constr) &&
	  !LSQMatrix::getCArray(error, in, constr, lInt,
				constr_p)) return False;
      if (in.isDefined(known) &&
	  !LSQMatrix::getCArray(error, in, known, n_p,
				known_p)) return False;
      if (in.isDefined(errors) &&
	  !LSQMatrix::getCArray(error, in, errors, lInt,
				error_p)) return False;
      if (in.isDefined(sol) &&
	  !LSQMatrix::getCArray(error, in, sol, n_p, sol_p)) return False;
      if (in.isDefined(lar) &&
	  !LSQMatrix::getCArray(error, in, lar, lInt,
				lar_p)) return False;
      if (in.isDefined(wsol) &&
	  !LSQMatrix::getCArray(error, in, wsol, n_p, wsol_p)) return False;
      if (in.isDefined(wcov) &&
	  !LSQMatrix::getCArray(error, in, wcov, lInt,
				wcov_p)) return False;
      if (in.isDefined(nar)) {
	nar_p = new LSQFit;
	if (!nar_p->fromRecord(error, in.asRecord(RecordFieldId(nar)))) {
	  return False;
	};
      };
    } else {
      error += String("Incorrect fields for fitting record");
      return False;
   };

    return True;
  }
  
  const String &LSQFit::ident() const {
    static String myid = "lfit";
    return myid;
  }

} //# NAMESPACE CASA - END

