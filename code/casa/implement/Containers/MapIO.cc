//# MapIO.cc: Classes to perform IO for Map classas
//# Copyright (C) 1993,1994,1995
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


#include <aips/Containers/MapIO.h>
#include <aips/IO/AipsIO.h>

template<class key, class value> AipsIO &operator<<(AipsIO &ios, const Map<key,value> &map) {
  ConstMapIter<key,value> mapP(map);
  ios.putstart (rtti_decode(map.id()), Map<key,value>::MapVersion);
  mapP.toStart();
  ios << map.ndefined();
  ios << map.defaultVal();
  while (mapP.atEnd() != True) {
    ios << mapP.getKey();
    ios << mapP.getVal();
    mapP++;
  }
  ios.putend ();
  return ios;
}

template<class key, class value> AipsIO &operator>>(AipsIO &ios, Map<key,value> &map) {
  key k;
  int len,i;
  int vers = ios.getstart(rtti_decode(map.id()));

  map.clear();
  ios >> len;
  ios >> map.defaultVal();
  for (i=0; i<len; i++) {
    ios >> k;
    ios >> map(k);
  }
  ios.getend ();

  return ios;
}

template<class key, class value> AipsIO &operator<<(AipsIO &ios, const ConstMapIter<key,value> &map) {

  return operator<<(ios,map.container());

}

template<class key, class value> AipsIO &operator>>(AipsIO &ios, MapIter<key,value> &map) {

  return operator>>(ios,map.container());

}

template<class key, class value> ostream &operator<<(ostream &ios, const Map<key,value> &map) {
#if !defined(AIPS_STUPID_SUN)
  ConstMapIter<key,value> mapP(map);
#else
  ConstMapIter<key,value> *mapPP = map.getIter();
  ConstMapIter<key,value> &mapP = *mapPP;
#endif
  mapP.toStart();
//ios << map.ndefined();
//ios << map.defaultVal();
  while (mapP.atEnd() != True) {
    ios << "(";
    ios << mapP.getKey();
    ios << ",";
    ios << mapP.getVal();
    ios << ")";
    mapP++;
  }
#if defined(AIPS_STUPID_SUN)
  delete mapPP;
#endif
  return ios;
}

template<class key, class value> ostream &operator<<(ostream &ios, const ConstMapIter<key,value> &map) {

  return operator<<(ios,map.container());

}
