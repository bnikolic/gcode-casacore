//# VisibilityIterator.cc: Step through MeasurementEquation by visibility
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

#include <trial/MeasurementEquations/VisibilityIterator.h>
#include <trial/MeasurementEquations/VisBuffer.h>
#include <aips/Arrays/ArrayLogical.h>
#include <aips/Arrays/ArrayMath.h>
#include <aips/Arrays/MaskedArray.h>
#include <aips/Exceptions/Error.h>
#include <aips/Functionals/ScalarSampledFunctional.h>
#include <aips/Functionals/Interpolate1D.h>
#include <aips/MeasurementSets/MSColumns.h>
#include <aips/Measures/MVTime.h>
#include <aips/Tables/TableDesc.h>
#include <aips/Tables/ColDescSet.h>
#include <aips/Tables/TableRecord.h>

ROVisibilityIterator::ROVisibilityIterator() {}

// const of MS is cast away, but we don't actually change it.
// (just to share code between RO version and RW version of iterator)
ROVisibilityIterator::ROVisibilityIterator(const MeasurementSet &ms,
					   const Block<Int>& sortColumns,
					   Double timeInterval)
: nChan_p(0),lastUT_p(0),curChanGroup_p(0),freqCacheOK_p(False),
initialized_p(False),velSelection_p(False)
{
  Block<MeasurementSet> mss(1); mss[0]=ms;
  msIter_p=MSIter(mss,sortColumns,timeInterval);
  This = (ROVisibilityIterator*)this; 
}

ROVisibilityIterator::ROVisibilityIterator(const Block<MeasurementSet> &mss,
					   const Block<Int>& sortColumns,
					   Double timeInterval)
: nChan_p(0),lastUT_p(0),curChanGroup_p(0),freqCacheOK_p(False),
initialized_p(False),msIter_p(mss,sortColumns,timeInterval),
velSelection_p(False)
{
  This = (ROVisibilityIterator*)this; 
}

ROVisibilityIterator::ROVisibilityIterator(const ROVisibilityIterator& other)
{
    operator=(other);
}

ROVisibilityIterator::~ROVisibilityIterator() {}

ROVisibilityIterator& 
ROVisibilityIterator::operator=(const ROVisibilityIterator& other) 
{
  This=(ROVisibilityIterator*)this;
  msIter_p=other.msIter_p;
  selTable_p=other.selTable_p;
  curChanGroup_p=other.curChanGroup_p;
  curNumChanGroup_p=other.curNumChanGroup_p;
  channelGroupSize_p=other.channelGroupSize_p;
  curNumRow_p=other.curNumRow_p;
  curTableNumRow_p=other.curTableNumRow_p;
  curStartRow_p=other.curStartRow_p;
  curEndRow_p=other.curEndRow_p;
  nChan_p=other.nChan_p;
  nPol_p=other.nPol_p;
  more_p=other.more_p;
  newChanGroup_p=other.newChanGroup_p;
  initialized_p=other.initialized_p;
  numChanGroup_p=other.numChanGroup_p;
  chanStart_p=other.chanStart_p;
  chanWidth_p=other.chanWidth_p;
  chanInc_p=other.chanInc_p;
  preselectedChanStart_p=other.preselectedChanStart_p;
  preselectednChan_p=other.preselectednChan_p;
  
  slicer_p=other.slicer_p;
  weightSlicer_p=other.weightSlicer_p;
  useSlicer_p=other.useSlicer_p;
  time_p.resize(other.time_p.nelements()); 
  time_p=other.time_p;
  frequency_p.resize(other.frequency_p.nelements()); 
  frequency_p=other.frequency_p;
  freqCacheOK_p=other.freqCacheOK_p;
  flagOK_p = other.flagOK_p;
  visOK_p = other.visOK_p;
  weightSpOK_p = other.weightSpOK_p;
  flagCube_p.resize(other.flagCube_p.shape()); flagCube_p=other.flagCube_p;
  visCube_p.resize(other.visCube_p.shape()); visCube_p=other.visCube_p;
  uvwMat_p.resize(other.uvwMat_p.shape()); uvwMat_p=other.uvwMat_p;
  pa_p.resize(other.pa_p.nelements()); pa_p=other.pa_p;

  msd_p=other.msd_p;
  lastUT_p=other.lastUT_p;
  nAnt_p=other.nAnt_p;

  velSelection_p = other.velSelection_p;
  vPrecise_p=other.vPrecise_p;
  nVelChan_p=other.nVelChan_p;
  vStart_p=other.vStart_p;
  vInc_p=other.vInc_p;
  // NOTE: following causes a seg violation
  cFromBETA_p=other.cFromBETA_p;
  vDef_p=other.vDef_p;
  // use this instead of assignment for the moment
  //cFromBETA_p.set(MDoppler(MVDoppler(Quantity(0.,"m/s")),
  //			   MDoppler::BETA),vDef_p);
  selFreq_p.resize(other.selFreq_p.nelements()); selFreq_p=other.selFreq_p;

  // column access functions
  colAntenna1.reference(other.colAntenna1);
  colAntenna2.reference(other.colAntenna2);
  colTime.reference(other.colTime);
  colWeight.reference(other.colWeight);
  colWeightSpectrum.reference(other.colWeightSpectrum);
  colVis.reference(other.colVis);
  colSigma.reference(other.colSigma);
  colFlag.reference(other.colFlag);
  colFlagRow.reference(other.colFlagRow);
  colUVW.reference(other.colUVW);

  return *this;
}

void ROVisibilityIterator::origin()
{
  if (!initialized_p) {
    originChunks();
  } else {
    curChanGroup_p=0;
    newChanGroup_p=True;
    curStartRow_p=0;
    freqCacheOK_p=False;
    flagOK_p = visOK_p = weightSpOK_p = False;
    setSelTable();
    updateSlicer();
    more_p=ToBool(curChanGroup_p<curNumChanGroup_p);
    // invalidate any attached VisBuffer
    if (!vbStack_p.empty()) ((VisBuffer*)vbStack_p.top())->invalidate();
  }
}

void ROVisibilityIterator::originChunks()
{
  initialized_p=True;
  msIter_p.origin();
  setState();
  origin();
}

void ROVisibilityIterator::advance()
{
  newChanGroup_p=False;
  flagOK_p = visOK_p = weightSpOK_p = False;
  curStartRow_p=curEndRow_p+1;
  if (curStartRow_p>=curTableNumRow_p) {
    if (++curChanGroup_p >= curNumChanGroup_p) {
      curChanGroup_p--;
      more_p=False;
    } else {
      curStartRow_p=0;
      newChanGroup_p=True;
      freqCacheOK_p=False;
      updateSlicer();
    }
  }
  if (more_p) {
    setSelTable();
    // invalidate any attached VisBuffer
    if (!vbStack_p.empty()) ((VisBuffer*)vbStack_p.top())->invalidate();
  }
}

ROVisibilityIterator& ROVisibilityIterator::nextChunk()
{
  if (msIter_p.more()) msIter_p++;
  if (msIter_p.more()) {
    setState();
    if (!vbStack_p.empty()) ((VisBuffer*)vbStack_p.top())->invalidate();
  }
  more_p=msIter_p.more();
  return *this;
}

void ROVisibilityIterator::setSelTable()
{
  // work out how many rows to return 
  // for the moment we return all rows with the same value for time
  for (curEndRow_p=curStartRow_p+1; curEndRow_p<curTableNumRow_p && 
  	 time_p(curEndRow_p)==time_p(curEndRow_p-1); 
       curEndRow_p++);
  curEndRow_p--;

  curNumRow_p=curEndRow_p-curStartRow_p+1;
  Vector<uInt> rows(curNumRow_p);
  indgen(rows.ac(),uInt(curStartRow_p));
  selTable_p=msIter_p.table()(rows);
  // virtual call
  this->attachColumns();
}

void ROVisibilityIterator::setState()
{
  curTableNumRow_p = msIter_p.table().nrow();
  // get the times for this iteration
  ScalarColumn<Double> colTime(msIter_p.table(),MS::columnName(MS::TIME));
  time_p.resize(curTableNumRow_p); colTime.getColumn(time_p);
  curStartRow_p=0;
  setSelTable();
  if ( msIter_p.newSpectralWindow()) {
    Int spw=msIter_p.spectralWindowId();
    nChan_p = colVis.shape(0)(1);
    nPol_p = colVis.shape(0)(0);
    if (numChanGroup_p.nelements()<= spw || 
	numChanGroup_p[spw] == 0) {
      // no selection set yet, set default = all
      // for a reference MS this will normally be set appropriately in VisSet
      selectChannel(1,msIter_p.startChan(),nChan_p);
    }
    channelGroupSize_p=chanWidth_p[spw];
    curNumChanGroup_p=numChanGroup_p[spw];
    freqCacheOK_p=False;
    if (velSelection_p) {
      // convert selected velocities to TOPO frequencies
      // first convert observatory vel to correct frame (for this time)
      MEpoch epoch = MS::epochMeasure(colTime);
      epoch.set(MVEpoch(Quantity(time_p(0),"s")));
      msd_p.setEpoch(epoch);
      msd_p.setAntenna(-1); // set the observatory position
      // get obs velocity in required frame
      MRadialVelocity obsRV = msd_p.obsVel();
      // convert to doppler in required definition and get out in m/s
      Double obsVel=cFromBETA_p(obsRV.toDoppler()).getValue().get().getValue();
      // Now compute corresponding TOPO freqs
      selFreq_p.resize(nVelChan_p);
      Double v0 = vStart_p.getValue(), dv=vInc_p.getValue();
      for (Int i=0; i<nVelChan_p; i++) {
	Double vTopo = v0 + i*dv - obsVel;
	MDoppler dTopo(Quantity(vTopo,"m/s"), vDef_p);
	selFreq_p(i) = MFrequency::fromDoppler
	  (dTopo,msIter_p.restFrequency().getValue()).getValue().getValue();
      }
    }
  }
  if (msIter_p.newField()) { 
    msd_p.setFieldCenter(msIter_p.phaseCenter());
  }
  // If this is a new array then set up the antenna locations
  if (msIter_p.newArray()) {
    This->nAnt_p = msd_p.setAntennas(msIter_p.msColumns().antenna(),
				     msIter_p.arrayId());
    This->pa_p.resize(nAnt_p);
  }	
}

void ROVisibilityIterator::updateSlicer()
{
  // set the Slicer to get the selected part of spectrum out of the table
  Int spw=msIter_p.spectralWindowId();
  Int start=chanStart_p[spw]+curChanGroup_p*chanInc_p[spw];
  start-=msIter_p.startChan();
  AlwaysAssert(start>=0 && start+channelGroupSize_p<=nChan_p,AipsError);
  //  slicer_p=Slicer(Slice(),Slice(start,channelGroupSize_p));
  // above is slow, use IPositions instead..
  slicer_p=Slicer(IPosition(2,0,start),
		  IPosition(2,nPol_p,channelGroupSize_p));
  weightSlicer_p=Slicer(IPosition(1,start),IPosition(1,channelGroupSize_p));
  useSlicer_p=ToBool(channelGroupSize_p<nChan_p);
}

void ROVisibilityIterator::attachColumns()
{
  colAntenna1.attach(selTable_p,MS::columnName(MS::ANTENNA1));
  colAntenna2.attach(selTable_p,MS::columnName(MS::ANTENNA2));
  colTime.attach(selTable_p,MS::columnName(MS::TIME));
  colVis.attach(selTable_p,MS::columnName(MS::DATA));
  colUVW.attach(selTable_p,MS::columnName(MS::UVW));
  colFlag.attach(selTable_p,MS::columnName(MS::FLAG));
  colFlagRow.attach(selTable_p,MS::columnName(MS::FLAG_ROW));
  colSigma.attach(selTable_p,MS::columnName(MS::SIGMA));
  colWeight.attach(selTable_p,MS::columnName(MS::WEIGHT));
  const ColumnDescSet& cds=selTable_p.tableDesc().columnDescSet();
  if (cds.isDefined(MS::columnName(MS::WEIGHT_SPECTRUM))) 
    colWeightSpectrum.attach(selTable_p,MS::columnName(MS::WEIGHT_SPECTRUM));
}

ROVisibilityIterator & ROVisibilityIterator::operator++(int)
{
  if (!more_p) return *this;
  advance();
  return *this;
}

ROVisibilityIterator & ROVisibilityIterator::operator++()
{
  if (!more_p) return *this;
  advance();
  return *this;
}

Vector<Int>& ROVisibilityIterator::antenna1(Vector<Int>& ant1) const
{
  ant1.resize(curNumRow_p);
  colAntenna1.getColumn(ant1);
  return ant1;
}

Vector<Int>& ROVisibilityIterator::antenna2(Vector<Int>& ant2) const
{
  ant2.resize(curNumRow_p);
  colAntenna2.getColumn(ant2);
  return ant2;
}


Vector<Int>& ROVisibilityIterator::channel(Vector<Int>& chan) const
{
  Int spw = msIter_p.spectralWindowId();
  chan.resize(channelGroupSize_p);
  for (Int i=0; i<channelGroupSize_p; i++) {
    chan(i)=chanStart_p[spw]+curChanGroup_p*chanInc_p[spw]+i;
  }
  return chan;
}

Vector<Int>& ROVisibilityIterator::corrType(Vector<Int>& corrTypes) const
{
  Int spw = msIter_p.spectralWindowId();
  msIter_p.msColumns().spectralWindow().corrType().get(spw,corrTypes,True);
  return corrTypes;
}

Cube<Bool>& ROVisibilityIterator::flag(Cube<Bool>& flags) const
{
  if (velSelection_p) {
    if (!flagOK_p) {
      // need to do the interpolation
      getInterpolatedVisFlagWeight();
    }
    flags.resize(flagCube_p.shape());  flags=flagCube_p; 
  } else {
    if (useSlicer_p) colFlag.getColumn(slicer_p,flags,True);
    else colFlag.getColumn(flags,True);
  }
  return flags;
}

Matrix<Bool>& ROVisibilityIterator::flag(Matrix<Bool>& flags) const
{
  if (useSlicer_p) colFlag.getColumn(slicer_p,This->flagCube_p,True);
  else colFlag.getColumn(This->flagCube_p,True);
  flags.resize(channelGroupSize_p,curNumRow_p);
  // need to optimize this...
  //for (Int row=0; row<curNumRow_p; row++) {
  //  for (Int chn=0; chn<channelGroupSize_p; chn++) {
  //    flags(chn,row)=flagCube(0,chn,row);
  //    for (Int pol=1; pol<nPol_p; pol++) {
  //	  flags(chn,row)|=flagCube(pol,chn,row);
  //    }
  //  }
  //}
  Bool deleteIt1;
  Bool deleteIt2;
  const Bool* pcube=This->flagCube_p.getStorage(deleteIt1);
  Bool* pflags=flags.getStorage(deleteIt2);
  for (Int row=0; row<curNumRow_p; row++) {
    for (Int chn=0; chn<channelGroupSize_p; chn++) {
      *pflags=*pcube++;
      for (Int pol=1; pol<nPol_p; pol++, pcube++) {
	*pflags = *pcube ? *pcube : *pflags;
      }
      pflags++;
    }
  }
  flagCube_p.freeStorage(pcube, deleteIt1);
  flags.putStorage(pflags, deleteIt2);
  return flags;
}

Vector<Bool>& ROVisibilityIterator::flagRow(Vector<Bool>& rowflags) const
{
  rowflags.resize(curNumRow_p);
  colFlagRow.getColumn(rowflags);
  return rowflags;
}

Vector<Double>& ROVisibilityIterator::frequency(Vector<Double>& freq) const
{
  if (!freqCacheOK_p) {
    This->freqCacheOK_p=True;
    Int spw = msIter_p.spectralWindowId();
    This->frequency_p.resize(channelGroupSize_p);
    const Vector<Double>& chanFreq=msIter_p.frequency();
    Int start=chanStart_p[spw]-msIter_p.startChan();
    for (Int i=0; i<channelGroupSize_p; i++) {
      This->frequency_p(i)=chanFreq(start+curChanGroup_p*chanInc_p[spw]+i);
    }
  }
  freq.resize(channelGroupSize_p);
  freq=frequency_p;
  return freq;
}

Vector<Double>& ROVisibilityIterator::time(Vector<Double>& t) const
{
  t.resize(curNumRow_p);
  colTime.getColumn(t); 
  return t;
}

Cube<Complex>& 
ROVisibilityIterator::visibility(Cube<Complex>& vis) const
{
  if (velSelection_p) {
    if (!visOK_p) {
      getInterpolatedVisFlagWeight();
    }
    vis.resize(visCube_p.shape()); vis=visCube_p;
  } else { 
    if (useSlicer_p) colVis.getColumn(slicer_p,vis,True);
    else colVis.getColumn(vis,True);
  }
  return vis;
}

void ROVisibilityIterator::getInterpolatedVisFlagWeight() const
{
  // get vis, flags & weights
  // tricky.. to avoid recursion we need to set velSelection_p to False
  // temporarily.
  This->velSelection_p = False; 
  visibility(This->visCube_p);
  flag(This->flagCube_p); 
  weightSpectrum(This->weightSpectrum_p);
  This->velSelection_p = True;

  // now interpolate visibilities, using selFreq as the sample points
  // we should have two options: flagging output points that have
  // any flagged inputs or interpolating across flagged data.
  Vector<Double> freq; frequency(freq);
  // convert frequencies to float (removing offset to keep accuracy) 
  // so we can multiply them with Complex numbers to do the interpolation.
  Vector<Float> xfreq(nChan_p),sfreq(nVelChan_p); 
  for (Int i=0; i<nChan_p; i++) xfreq(i)=freq(i)-freq(0);
  for (i=0; i<nVelChan_p; i++) sfreq(i)=selFreq_p(i)-freq(0);
  // set up the Functionals for the interpolation
  ScalarSampledFunctional<Float> x(xfreq);
  Vector<MaskedArray<Complex> > visPlanes(nChan_p);
  // we should probably be using the flags for weight interpolation as well
  // but it's not clear how to combine the 4 pol flags into one.
  // (AND the flags-> weight flagged if all flagged)
  Vector<Array<Float> > wtVectors(nChan_p);
  for (i=0; i<nChan_p; i++) {
    visPlanes(i).
      setData(This->visCube_p(Slice(),Slice(i,1),Slice()),
	      (This->flagCube_p(Slice(),Slice(i,1),Slice()).ac()==False));
    Array<Float> wtrow(This->weightSpectrum_p.row(i));
    wtVectors(i).reference(wtrow);
  }
  ScalarSampledFunctional<MaskedArray<Complex> > yVis(visPlanes);
  ScalarSampledFunctional<Array<Float> > yWt(wtVectors);
  Interpolate1D<Float,MaskedArray<Complex> > intVis(x,yVis,True,True);
  Interpolate1D<Float,Array<Float> > intWt(x,yWt,True,True);
  // set the interpolation method: nearest, linear, cubic, cubic spline

  IPosition shape(3,nPol_p,nVelChan_p,curNumRow_p);
  This->visCube_p.resize(shape);
  This->flagCube_p.resize(shape);
  This->visCube_p=Complex(0.); // to make masked values come out as zero.
  This->weightSpectrum_p.resize(nVelChan_p,curNumRow_p);
  // now do the actual interpolation
  for (i=0; i<nVelChan_p; i++) {
    MaskedArray<Complex> maskedVis(intVis(sfreq(i)));
    This->visCube_p(Slice(),Slice(i,1),Slice())=maskedVis;
    Array<Bool> flags=(maskedVis.getMask()==False);
    This->flagCube_p(Slice(),Slice(i,1),Slice())=flags;
    This->weightSpectrum_p.row(i)=intWt(sfreq(i));
  }
}

Matrix<CStokesVector>& 
ROVisibilityIterator::visibility(Matrix<CStokesVector>& vis) const
{
  if (useSlicer_p) colVis.getColumn(slicer_p,This->visCube_p,True);
  else colVis.getColumn(This->visCube_p,True);
  vis.resize(channelGroupSize_p,curNumRow_p);
  Bool deleteIt;
  Complex* pcube=This->visCube_p.getStorage(deleteIt);
  if (deleteIt) cerr<<"Problem in ROVisIter::visibility - deleteIt True"<<endl;
  // Here we cope in a limited way with cases where not all 4 
  // polarizations are present: if only 2, assume XX,YY or RR,LL
  // if only 1, assume it's an estimate of Stokes I (one of RR,LL,XX,YY)
  // The cross terms are zero filled in these cases.
  switch (nPol_p) {
  case 4: {
    for (Int row=0; row<curNumRow_p; row++) {
      for (Int chn=0; chn<channelGroupSize_p; chn++,pcube+=4) {
	vis(chn,row)=pcube;
      }
    }
    break;
  }
  case 2: {
    for (Int row=0; row<curNumRow_p; row++) {
      for (Int chn=0; chn<channelGroupSize_p; chn++,pcube+=2) {
	CStokesVector& v=vis(chn,row);
	v=Complex(0.,0.);
	v(0)=*pcube; 
	v(3)=*(pcube+1); 
      }
    }
    break;
  }
  case 1: {
    for (Int row=0; row<curNumRow_p; row++) {
      for (Int chn=0; chn<channelGroupSize_p; chn++,pcube++) {
	CStokesVector& v=vis(chn,row);
	v=Complex(0.,0.);
	v(0)=v(3)=*pcube; 
      }
    }
  } //# case 1
  } //# switch 
  return vis;
}

Vector<RigidVector<Double,3> >& 
ROVisibilityIterator::uvw(Vector<RigidVector<Double,3> >& uvwvec) const
{
    uvwvec.resize(curNumRow_p);
    colUVW.getColumn(This->uvwMat_p,True);
    // get a pointer to the raw storage for quick access
    Bool deleteIt;
    Double* pmat=This->uvwMat_p.getStorage(deleteIt);
    for (Int row=0; row<curNumRow_p; row++, pmat+=3) uvwvec(row)=pmat;
    return uvwvec;
}

// Fill in parallactic angle.
const Vector<Float>& ROVisibilityIterator::feed_pa(Double time) const
{
  //  LogMessage message(LogOrigin("ROVisibilityIterator","feed_pa"));

  // Absolute UT
  Double ut=time;

  if (ut!=lastUT_p) {
    This->lastUT_p=ut;

    // Set up the Epoch using the absolute MJD in seconds
    // get the Epoch reference from the column keyword
        MEpoch mEpoch=MS::epochMeasure(colTime);
    //#     now set the value
        mEpoch.set(MVEpoch(Quantity(ut, "s")));
    //# Note the above fails for VLA data with TAI epoch, the resulting
    //# pa's are wrong (by much more than UTC-TAI), we force UTC here 
    //# for the moment
    //    MEpoch mEpoch(Quantity(ut, "s"), MEpoch::Ref(MEpoch::UTC));

    This->msd_p.setEpoch(mEpoch);

    // Calculate pa for all antennas.
    for (Int iant=0;iant<nAnt_p;iant++) {
      This->msd_p.setAntenna(iant);
      This->pa_p(iant) = This->msd_p.parAngle();
      // add angle for receptor 0
      This->pa_p(iant)+= msIter_p.receptorAngle()(0,iant);
      if (aips_debug) {
	if (iant==0) 
	  cout<<"Antenna "<<iant<<" at time: "<<MVTime(mEpoch.getValue())<<
	  " has PA = "<<This->pa_p(iant)*57.28<<endl;
      }
    }
  }
  return pa_p;
}

Vector<Float>& ROVisibilityIterator::sigma(Vector<Float>& sig) const
{
  Matrix<Float> sigmat=colSigma.getColumn();
  // Do a rough average of the parallel hand polarizations to get a single 
  // sigma. Should do this properly someday, or return all values
  sig.resize(sigmat.ncolumn());
  sig.ac()=sigmat.row(0).ac();
  sig.ac()+=sigmat.row(nPol_p-1).ac();
  sig.ac()/=2.0f;
  return sig;
}

Vector<Float>& ROVisibilityIterator::weight(Vector<Float>& wt) const
{
  wt.resize(curNumRow_p);
  colWeight.getColumn(wt);
  return wt;
}

Matrix<Float>& ROVisibilityIterator::weightSpectrum(Matrix<Float>& wt) const
{
  if (velSelection_p) {
    if (!weightSpOK_p) {
      getInterpolatedVisFlagWeight();
    }
    wt.resize(weightSpectrum_p.shape()); wt=weightSpectrum_p; 
  } else {
    if (useSlicer_p) colWeightSpectrum.getColumn(weightSlicer_p,wt,True);
    else colWeightSpectrum.getColumn(wt,True);
  }
  return wt;
}

ROVisibilityIterator& 
ROVisibilityIterator::selectVelocity
(Int nChan, const MVRadialVelocity& vStart, const MVRadialVelocity& vInc,
 MRadialVelocity::Types rvType, MDoppler::Types dType, Bool precise)
{
  velSelection_p=True;
  nVelChan_p=nChan;
  vStart_p=vStart;
  vInc_p=vInc;
  msd_p.setVelocityFrame(rvType);
  vDef_p=dType;
  cFromBETA_p.set(MDoppler(MVDoppler(Quantity(0.,"m/s")),
			   MDoppler::BETA),vDef_p);
  vPrecise_p=precise;
  if (precise) {
    // set up conversion engine for full conversion
  }
  // have to reset the iterator so all caches get filled
  originChunks();
}


ROVisibilityIterator& 
ROVisibilityIterator::selectChannel(Int nGroup, Int start, Int width, 
				    Int increment, Int spectralWindow)
{
  Int spw=spectralWindow;
  if (spw<0) spw = msIter_p.spectralWindowId();
  Int n = numChanGroup_p.nelements();
  if (spw >= n) {
    // we need to resize the blocks
    Int newn = max(2,max(2*n,spw+1));
    numChanGroup_p.resize(newn);
    chanStart_p.resize(newn);
    chanWidth_p.resize(newn);
    chanInc_p.resize(newn);
    for (Int i = n; i<newn; i++) numChanGroup_p[i] = 0;
  }
  chanStart_p[spw] = start;
  chanWidth_p[spw] = width;
  chanInc_p[spw] = increment;
  numChanGroup_p[spw] = nGroup;
  return *this;
}

void ROVisibilityIterator::attachVisBuffer(VisBuffer& vb)
{
  vbStack_p.push((void*)&vb);
  vb.invalidate();
}

void ROVisibilityIterator::detachVisBuffer(VisBuffer& vb)
{
  if (!vbStack_p.empty()) {
    if ((void*)vbStack_p.top() == (void*)&vb) {
      vbStack_p.pop();
      if (!vbStack_p.empty()) ((VisBuffer*)vbStack_p.top())->invalidate();
    } else {
      throw(AipsError("ROVisIter::detachVisBuffer - attempt to detach "
		      "buffer that is not the last one attached"));
    }
  }
}

VisibilityIterator::VisibilityIterator() {}

VisibilityIterator::VisibilityIterator(MeasurementSet &MS, 
				       const Block<Int>& sortColumns, 
				       Double timeInterval)
:ROVisibilityIterator(MS, sortColumns, timeInterval)
{
}

VisibilityIterator::VisibilityIterator(const VisibilityIterator & other)
{
    operator=(other);
}

VisibilityIterator::~VisibilityIterator() {}

VisibilityIterator& 
VisibilityIterator::operator=(const VisibilityIterator& other)
{
    if (this!=&other) {
	ROVisibilityIterator::operator=(other);
	RWcolFlag.reference(other.RWcolFlag);
	RWcolVis.reference(other.RWcolVis);
	RWcolWeight.reference(other.RWcolWeight);
	RWcolWeightSpectrum.reference(other.RWcolWeightSpectrum);
    }
    return *this;
}

VisibilityIterator & VisibilityIterator::operator++(int)
{
  if (!more_p) return *this;
  advance();
  return *this;
}

VisibilityIterator & VisibilityIterator::operator++()
{
  if (!more_p) return *this;
  advance();
  return *this;
}

void VisibilityIterator::attachColumns()
{
  ROVisibilityIterator::attachColumns();
  RWcolVis.attach(selTable_p,MS::columnName(MS::DATA));
  RWcolWeight.attach(selTable_p,MS::columnName(MS::WEIGHT));
  RWcolFlag.attach(selTable_p,MS::columnName(MS::FLAG));
  //todo: should cache this (update once per ms)
  const ColumnDescSet& cds=selTable_p.tableDesc().columnDescSet();
  if (cds.isDefined(MS::columnName(MS::WEIGHT_SPECTRUM))) 
    RWcolWeightSpectrum.attach(selTable_p,MS::columnName(MS::WEIGHT_SPECTRUM));
}
void VisibilityIterator::setFlag(const Matrix<Bool>& flag)
{
  // use same value for all polarizations
  //#flagCube_p.resize(nPol_p,channelGroupSize_p,curNumRow_p);
  Bool deleteIt;
  Bool* p=flagCube_p.getStorage(deleteIt);
  //Matrix<Bool> flagmat(nPol_p,channelGroupSize_p);
  //Bool* pflagmat=flagmat.getStorage(deleteIt);
  const Bool* pflag=flag.getStorage(deleteIt);
  if (flag.nrow()!=channelGroupSize_p) {
    throw(AipsError("VisIter::setFlag(flag) - inconsistent number of channels"));
  }
  
  for (Int row=0; row<curNumRow_p; row++) {
    //    Bool* p=pflagmat;
    for (Int chn=0; chn<channelGroupSize_p; chn++) {
      for (Int pol=0; pol<nPol_p; pol++) {
	*p++=*pflag;
      }
      pflag++;
    }
    //    if (useSlicer_p) RWcolFlag.putSlice(row,slicer_p,flagmat);
    //    else RWcolFlag.put(row,flagmat);
  }
  //#putColumn fails
  if (useSlicer_p) RWcolFlag.putColumn(slicer_p,flagCube_p);
  else RWcolFlag.putColumn(flagCube_p);
}

void VisibilityIterator::setFlag(const Cube<Bool>& flags)
{
  if (useSlicer_p) RWcolFlag.putColumn(slicer_p,flags);
  else RWcolFlag.putColumn(flags);
}

void VisibilityIterator::setVis(const Matrix<CStokesVector> & vis)
{
  // two problems: 1. channel selection -> we can only write to reference
  // MS with 'processed' channels
  //               2. polarization: there could be 1, 2 or 4 in the
  // original data, predict() always gives us 4. We save what was there
  // originally.

  //  if (!preselected_p) {
  //    throw(AipsError("VisIter::setVis(vis) - cannot change original data"));
  //  }
  if (vis.nrow()!=channelGroupSize_p) {
    throw(AipsError("VisIter::setVis(vis) - inconsistent number of channels"));
  }
  // we need to reform the vis matrix to a cube before we can use
  // putColumn to a Matrix column
  //# putColumn fails at present, use row by row put instead
  //# putColumn now claimed to work, try it out 1997/05/25
  visCube_p.resize(nPol_p,channelGroupSize_p,curNumRow_p);
  //Matrix<Complex> visMat(nPol_p,channelGroupSize_p);
  Bool deleteIt;
  Complex* p=visCube_p.getStorage(deleteIt);
  //Complex* pvisMat=visMat.getStorage(deleteIt);
  for (Int row=0; row<curNumRow_p; row++) {
    //    Complex* p=pvisMat;
    for (Int chn=0; chn<channelGroupSize_p; chn++) {
      const CStokesVector& v=vis(chn,row);
      switch (nPol_p) {
      case 4: *p++=v(0); *p++=v(1); *p++=v(2); *p++=v(3); break;     
      case 2: *p++=v(0); *p++=v(3); break;
      case 1: *p++=(v(0)+v(3))/2; break;
      }
    }
    //    if (useSlicer_p) RWcolVis.putSlice(row,slicer_p,visMat);
    //    else RWcolVis.put(row,visMat);
  }
  if (useSlicer_p) RWcolVis.putColumn(slicer_p,visCube_p);
  else RWcolVis.putColumn(visCube_p);
}

void VisibilityIterator::setVis(const Cube<Complex>& vis)
{
  if (useSlicer_p) RWcolVis.putColumn(slicer_p,vis);
  else RWcolVis.putColumn(vis);
}

void VisibilityIterator::setWeight(const Vector<Float>& weight)
{
    RWcolWeight.putColumn(weight);
}

void VisibilityIterator::setWeightSpectrum(const Matrix<Float>& wt)
{
  if (useSlicer_p) RWcolWeightSpectrum.putColumn(slicer_p,wt);
  else RWcolWeightSpectrum.putColumn(wt);
}






