/*============================================================================
*
*   WCSLIB 3.4 - an implementation of the FITS WCS convention.
*   Copyright (C) 1995-2004, Mark Calabretta
*
*   This library is free software; you can redistribute it and/or modify it
*   under the terms of the GNU Library General Public License as published
*   by the Free Software Foundation; either version 2 of the License, or (at
*   your option) any later version.
*
*   This library is distributed in the hope that it will be useful, but
*   WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library
*   General Public License for more details.
*
*   You should have received a copy of the GNU Library General Public License
*   along with this library; if not, write to the Free Software Foundation,
*   Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*
*   Correspondence concerning WCSLIB may be directed to:
*      Internet email: mcalabre@atnf.csiro.au
*      Postal address: Dr. Mark Calabretta
*                      Australia Telescope National Facility, CSIRO
*                      PO Box 76
*                      Epping NSW 1710
*                      AUSTRALIA
*
*   Author: Mark Calabretta, Australia Telescope National Facility
*   $Id$
*===========================================================================*/

#include <math.h>
#include <stdio.h>
#include <string.h>

#include "wcsmath.h"
#include "wcstrig.h"
#include "sph.h"
#include "wcs.h"

#if defined(__convexc__) || defined(__APPLE__)
#include <stdlib.h>
#else
#include <malloc.h>
#endif

const int WCSSET = 137;

/* Map error number to error message for each function. */
const char *wcs_errmsg[] = {
   0,
   "Null wcsprm pointer passed",
   "Memory allocation error",
   "Linear transformation matrix is singular",
   "Inconsistent or unrecognized coordinate axis types",
   "Invalid parameter value",
   "Invalid coordinate transformation parameters",
   "Ill-conditioned coordinate transformation parameters",
   "One or more of the pixel coordinates were invalid",
   "One or more of the world coordinates were invalid",
   "Invalid world coordinate",
   "No solution found in the specified interval"};

#define signbit(X) ((X) < 0.0 ? 1 : 0)

/*--------------------------------------------------------------------------*/

int wcsini(alloc, naxis, wcs)

int alloc, naxis;
struct wcsprm *wcs;

{
   int i, j, k, mem, status;
   double *cd;

   if (wcs == 0) return 1;

   if (naxis <= 0) {
      return 2;
   }

   if (wcs->flag == -1 || wcs->m_flag != WCSSET) {
      wcs->m_flag  = 0;
      wcs->m_naxis = 0;
      wcs->m_crpix = 0;
      wcs->m_pc    = 0;
      wcs->m_cdelt = 0;
      wcs->m_ctype = 0;
      wcs->m_cunit = 0;
      wcs->m_crval = 0;
      wcs->m_pv    = 0;
      wcs->m_cd    = 0;
      wcs->m_crota = 0;
   }

   if (wcs->flag == -1) {
      wcs->lin.flag = -1;
   }


   /* Allocate memory for arrays if required. */
   if (alloc ||
       wcs->crpix == (double *)0 ||
       wcs->pc    == (double *)0 ||
       wcs->cdelt == (double *)0 ||
       wcs->ctype == (char (*)[72])0 ||
       wcs->cunit == (char (*)[72])0 ||
       wcs->crval == (double *)0 ||
       wcs->pv    == (struct pvcard *)0 ||
       wcs->cd    == (double *)0 ||
       wcs->crota == (double *)0) {

      /* Was sufficient allocated previously? */
      if (wcs->m_flag == WCSSET && wcs->m_naxis < naxis) {
         /* No, free it. */
         wcsfree(wcs);
      }


      if (alloc || wcs->crpix == (double *)0) {
         if (wcs->m_crpix) {
            /* In case the caller fiddled with it. */
            wcs->crpix = wcs->m_crpix;

         } else {
            mem = naxis * sizeof(double);
            if ((wcs->crpix = (double *)malloc(mem)) == (double *)0) {
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_crpix = wcs->crpix;
         }
      }

      if (alloc || wcs->pc == (double *)0) {
         if (wcs->m_pc) {
            /* In case the caller fiddled with it. */
            wcs->pc = wcs->m_pc;

         } else {
            mem = naxis * naxis * sizeof(double);
            if ((wcs->pc = (double *)malloc(mem)) == (double *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_pc    = wcs->pc;
         }
      }

      if (alloc || wcs->cdelt == (double *)0) {
         if (wcs->m_cdelt) {
            /* In case the caller fiddled with it. */
            wcs->cdelt = wcs->m_cdelt;

         } else {
            mem = naxis * sizeof(double);
            if ((wcs->cdelt = (double *)malloc(mem)) == (double *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_cdelt = wcs->cdelt;
         }
      }

      if (alloc || wcs->ctype == (char (*)[72])0) {
         if (wcs->m_ctype) {
            /* In case the caller fiddled with it. */
            wcs->ctype = wcs->m_ctype;

         } else {
            mem = naxis*72*sizeof(char);
            if ((wcs->ctype = (char (*)[72])malloc(mem)) == (char (*)[72])0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_ctype = wcs->ctype;
         }
      }

      if (alloc || wcs->cunit == (char (*)[72])0) {
         if (wcs->m_cunit) {
            /* In case the caller fiddled with it. */
            wcs->cunit = wcs->m_cunit;

         } else {
            mem = naxis*72*sizeof(char);
            if ((wcs->cunit = (char (*)[72])malloc(mem)) == (char (*)[72])0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_cunit = wcs->cunit;
         }
      }

      if (alloc || wcs->crval == (double *)0) {
         if (wcs->m_crval) {
            /* In case the caller fiddled with it. */
            wcs->crval = wcs->m_crval;

         } else {
            mem = naxis*sizeof(double);
            if ((wcs->crval = (double *)malloc(mem)) == (double *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_crval = wcs->crval;
         }
      }

      if (alloc || wcs->pv == (struct pvcard *)0) {
         if (wcs->m_pv) {
            /* In case the caller fiddled with it. */
            wcs->pv = wcs->m_pv;

         } else {
            mem = NPVMAX*sizeof(struct pvcard);
            if ((wcs->pv = (struct pvcard *)malloc(mem)) ==
                           (struct pvcard *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_pv    = wcs->pv;
         }
      }

      if (alloc || wcs->cd == (double *)0) {
         if (wcs->m_cd) {
            /* In case the caller fiddled with it. */
            wcs->cd = wcs->m_cd;

         } else {
            mem = naxis * naxis * sizeof(double);
            if ((wcs->cd = (double *)malloc(mem)) == (double *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_cd    = wcs->cd;
         }
      }

      if (alloc || wcs->crota == (double *)0) {
         if (wcs->m_crota) {
            /* In case the caller fiddled with it. */
            wcs->crota = wcs->m_crota;

         } else {
            mem = naxis * sizeof(double);
            if ((wcs->crota = (double *)malloc(mem)) == (double *)0) {
               wcsfree(wcs);
               return 2;
            }

            wcs->m_flag  = WCSSET;
            wcs->m_naxis = naxis;
            wcs->m_crota = wcs->crota;
         }
      }
   }


   wcs->flag  = 0;
   wcs->naxis = naxis;


   /* Set defaults for the linear transformation. */
   wcs->lin.crpix  = wcs->crpix;
   wcs->lin.pc     = wcs->pc;
   wcs->lin.cdelt  = wcs->cdelt;
   wcs->lin.m_flag = 0;
   if (status = linini(0, naxis, &(wcs->lin))) {
      return status;
   }


   /* CTYPEi and CUNITi are blank by default. */
   for (i = 0; i < naxis; i++) {
      strcpy(wcs->ctype[i], "");
      strcpy(wcs->cunit[i], "");
   }


   /* CRVALi defaults to 0.0. */
   for (i = 0; i < naxis; i++) {
      wcs->crval[i] = 0.0;
   }


   /* Set defaults for the celestial transformation parameters. */
   wcs->lonpole = 999.0;
   wcs->latpole = 999.0;

   /* Set defaults for the spectral transformation parameters. */
   wcs->restfrq = 0.0;
   wcs->restwav = 0.0;

   /* Default parameter values. */
   wcs->npv = 0;
   for (k = 0; k < NPVMAX; k++) {
     wcs->pv[k].i = 0;
     wcs->pv[k].m = 0;
     wcs->pv[k].value = 0.0;
   }

   /* Defaults for alternate linear transformations. */
   wcs->altlin = 0;
   cd = wcs->cd;
   for (i = 0; i < naxis; i++) {
      for (j = 0; j < naxis; j++) {
         *(cd++) = 0.0;
      }
   }
   for (i = 0; i < naxis; i++) {
      wcs->crota[i] = 0.0;
   }


   /* Reset derived values. */
   strcpy(wcs->lngtyp, "    ");
   strcpy(wcs->lattyp, "    ");
   wcs->lng  = -1;
   wcs->lat  = -1;
   wcs->spec = -1;
   wcs->cubeface = -1;

   celini(&(wcs->cel));
   spcini(&(wcs->spc));

   return 0;
}

/*--------------------------------------------------------------------------*/

int wcscopy(alloc, wcssrc, wcsdst)

int alloc;
const struct wcsprm *wcssrc;
struct wcsprm *wcsdst;

{
   int i, j, k, naxis, status;
   const double *srcp;
   double *dstp;

   if (wcssrc == 0) return 1;

   if ((naxis = wcssrc->naxis) <= 0) {
      return 2;
   }

   if (status = wcsini(alloc, naxis, wcsdst)) {
      return status;
   }

   srcp = wcssrc->crpix;
   dstp = wcsdst->crpix;
   for (j = 0; j < naxis; j++) {
      *(dstp++) = *(srcp++);
   }

   srcp = wcssrc->pc;
   dstp = wcsdst->pc;
   for (i = 0; i < naxis; i++) {
      for (j = 0; j < naxis; j++) {
         *(dstp++) = *(srcp++);
      }
   }

   srcp = wcssrc->cdelt;
   dstp = wcsdst->cdelt;
   for (i = 0; i < naxis; i++) {
      *(dstp++) = *(srcp++);
   }

   for (i = 0; i < naxis; i++) {
      strncpy(wcsdst->ctype[i], wcssrc->ctype[i], 72);
      strncpy(wcsdst->cunit[i], wcssrc->cunit[i], 72);
   }

   srcp = wcssrc->crval;
   dstp = wcsdst->crval;
   for (i = 0; i < naxis; i++) {
      *(dstp++) = *(srcp++);
   }

   wcsdst->lonpole = wcssrc->lonpole;
   wcsdst->latpole = wcssrc->latpole;
   wcsdst->restfrq = wcssrc->restfrq;
   wcsdst->restwav = wcssrc->restwav;

   wcsdst->npv = wcssrc->npv;
   for (k = 0; k < NPVMAX; k++) {
     wcsdst->pv[k].i = wcssrc->pv[k].i;
     wcsdst->pv[k].m = wcssrc->pv[k].m;
     wcsdst->pv[k].value = wcssrc->pv[k].value;
   }

   wcsdst->altlin = wcssrc->altlin;

   srcp = wcssrc->cd;
   dstp = wcsdst->cd;
   for (i = 0; i < naxis; i++) {
      for (j = 0; j < naxis; j++) {
         *(dstp++) = *(srcp++);
      }
   }

   srcp = wcssrc->crota;
   dstp = wcsdst->crota;
   for (i = 0; i < naxis; i++) {
      *(dstp++) = *(srcp++);
   }


   return 0;
}

/*--------------------------------------------------------------------------*/

int wcsfree(wcs)

struct wcsprm *wcs;

{
   if (wcs == 0) return 1;

   /* Free memory allocated by wcsini(). */
   if (wcs->m_flag == WCSSET) {
      if (wcs->crpix == wcs->m_crpix) wcs->crpix = 0;
      if (wcs->pc    == wcs->m_pc)    wcs->pc    = 0;
      if (wcs->cdelt == wcs->m_cdelt) wcs->cdelt = 0;
      if (wcs->ctype == wcs->m_ctype) wcs->ctype = 0;
      if (wcs->cunit == wcs->m_cunit) wcs->cunit = 0;
      if (wcs->crval == wcs->m_crval) wcs->crval = 0;
      if (wcs->pv    == wcs->m_pv)    wcs->pv    = 0;
      if (wcs->cd    == wcs->m_cd)    wcs->cd    = 0;
      if (wcs->crota == wcs->m_crota) wcs->crota = 0;

      if (wcs->m_crpix) free(wcs->m_crpix);
      if (wcs->m_pc)    free(wcs->m_pc);
      if (wcs->m_cdelt) free(wcs->m_cdelt);
      if (wcs->m_ctype) free(wcs->m_ctype);
      if (wcs->m_cunit) free(wcs->m_cunit);
      if (wcs->m_crval) free(wcs->m_crval);
      if (wcs->m_pv)    free(wcs->m_pv);
      if (wcs->m_cd)    free(wcs->m_cd);
      if (wcs->m_crota) free(wcs->m_crota);
   }

   if (wcs->lin.crpix == wcs->m_crpix) wcs->lin.crpix = 0;
   if (wcs->lin.pc    == wcs->m_pc)    wcs->lin.pc    = 0;
   if (wcs->lin.cdelt == wcs->m_cdelt) wcs->lin.cdelt = 0;

   wcs->m_flag  = 0;
   wcs->m_naxis = 0;
   wcs->m_crpix = 0;
   wcs->m_pc    = 0;
   wcs->m_cdelt = 0;
   wcs->m_ctype = 0;
   wcs->m_cunit = 0;
   wcs->m_crval = 0;
   wcs->m_pv    = 0;
   wcs->m_cd    = 0;
   wcs->m_crota = 0;

   wcs->flag = 0;

   return linfree(&(wcs->lin));
}

/*--------------------------------------------------------------------------*/

int wcsprt(wcs)

const struct wcsprm *wcs;

{
   int i, j, k;

   if (wcs == 0) return 1;

   if (wcs->flag != WCSSET) {
      printf("The wcsprm struct is UNINITIALIZED.\n");
      return 0;
   }

   printf("       flag: %d\n", wcs->flag);
   printf("      naxis: %d\n", wcs->naxis);
   printf("      crpix: 0x%x\n", (int)wcs->crpix);
   printf("            ");
   for (i = 0; i < wcs->naxis; i++) {
      printf("  %- 11.4g", wcs->crpix[i]);
   }
   printf("\n");

   k = 0;
   printf("         pc: 0x%x\n", (int)wcs->pc);
   for (i = 0; i < wcs->naxis; i++) {
      printf("    pc[%d][]:", i);
      for (j = 0; j < wcs->naxis; j++) {
         printf("  %- 11.4g", wcs->pc[k++]);
      }
      printf("\n");
   }

   printf("      cdelt: 0x%x\n", (int)wcs->cdelt);
   printf("            ");
   for (i = 0; i < wcs->naxis; i++) {
      printf("  %- 11.4g", wcs->cdelt[i]);
   }
   printf("\n");

   printf("      ctype: 0x%x\n", (int)wcs->ctype);
   for (i = 0; i < wcs->naxis; i++) {
      printf("             \"%s\"\n", wcs->ctype[i]);
   }

   printf("      cunit: 0x%x\n", (int)wcs->cunit);
   for (i = 0; i < wcs->naxis; i++) {
      printf("             \"%s\"\n", wcs->cunit[i]);
   }

   printf("      crval: 0x%x\n", (int)wcs->crval);
   printf("            ");
   for (i = 0; i < wcs->naxis; i++) {
      printf("  %- 11.4g", wcs->crval[i]);
   }
   printf("\n");

   printf("    lonpole: %9f\n", wcs->lonpole);
   printf("    latpole: %9f\n", wcs->latpole);
   printf("    restfrq: %9f\n", wcs->restfrq);
   printf("    restwav: %9f\n", wcs->restwav);
   printf("        npv: %d\n",  wcs->npv);
   printf("         pv: 0x%x\n", (int)wcs->pv);
   for (i = 0; i < wcs->npv; i++) {
      printf("             %3d%4d  %- 11.4g\n", (wcs->pv[i]).i,
         (wcs->pv[i]).m, (wcs->pv[i]).value);
   }

   printf("     altlin: %d\n", wcs->altlin);

   k = 0;
   printf("         cd: 0x%x\n", (int)wcs->cd);
   for (i = 0; i < wcs->naxis; i++) {
      printf("    cd[%d][]:", i);
      for (j = 0; j < wcs->naxis; j++) {
         printf("  %- 11.4g", wcs->cd[k++]);
      }
      printf("\n");
   }

   printf("      crota: 0x%x\n", (int)wcs->crota);
   printf("            ");
   for (i = 0; i < wcs->naxis; i++) {
      printf("  %- 11.4g", wcs->crota[i]);
   }
   printf("\n");

   printf("     lngtyp: \"%s\"\n", wcs->lngtyp);
   printf("     lattyp: \"%s\"\n", wcs->lattyp);
   printf("        lng: %d\n", wcs->lng);
   printf("        lat: %d\n", wcs->lat);
   printf("       spec: %d\n", wcs->spec);
   printf("   cubeface: %d\n", wcs->cubeface);

   printf("        lin: (see below)\n");
   printf("        cel: (see below)\n");
   printf("        spc: (see below)\n");

   printf("     m_flag: %d\n", wcs->m_flag);
   printf("    m_naxis: %d\n", wcs->m_naxis);
   printf("    m_crpix: 0x%x", (int)wcs->m_crpix);
   if (wcs->m_crpix == wcs->crpix) printf("  (= crpix)");
   printf("\n");
   printf("       m_pc: 0x%x", (int)wcs->m_pc);
   if (wcs->m_pc == wcs->pc) printf("  (= pc)");
   printf("\n");
   printf("    m_cdelt: 0x%x", (int)wcs->m_cdelt);
   if (wcs->m_cdelt == wcs->cdelt) printf("  (= cdelt)");
   printf("\n");
   printf("    m_crval: 0x%x", (int)wcs->m_crval);
   if (wcs->m_crval == wcs->crval) printf("  (= crval)");
   printf("\n");
   printf("    m_ctype: 0x%x", (int)wcs->m_ctype);
   if (wcs->m_ctype == wcs->ctype) printf("  (= ctype)");
   printf("\n");
   printf("    m_cunit: 0x%x", (int)wcs->m_cunit);
   if (wcs->m_cunit == wcs->cunit) printf("  (= cunit)");
   printf("\n");
   printf("       m_pv: 0x%x", (int)wcs->m_pv);
   if (wcs->m_pv == wcs->pv) printf("  (= pv)");
   printf("\n");

   printf("\n");
   printf("   lin.*\n");
   linprt(&(wcs->lin));

   printf("\n");
   printf("   cel.*\n");
   celprt(&(wcs->cel));

   printf("\n");
   printf("   spc.*\n");
   spcprt(&(wcs->spc));

   return 0;
}

/*--------------------------------------------------------------------------*/

int wcsset(wcs)

struct wcsprm *wcs;

{
   const int  nalias = 2;
   const char aliases [2][4] = {"NCP", "GLS"};

   char ctypei[72], pcode[4], requir[9];
   int i, j, k, m, naxis, *ndx = 0, status;
   double lambda, rho;
   double *cd, *pc;
   struct celprm *wcscel = &(wcs->cel);
   struct prjprm *wcsprj = &(wcscel->prj);
   struct spcprm *wcsspc = &(wcs->spc);


   if (wcs == 0) return 1;

   /* Parse the CTYPEi cards. */
   strcpy(pcode, "");
   strcpy(requir, "");
   wcs->lng  = -1;
   wcs->lat  = -1;
   wcs->spec = -1;
   wcs->cubeface = -1;

   naxis = wcs->naxis;
   for (i = 0; i < naxis; i++) {
      strncpy(ctypei, wcs->ctype[i], 72);
      ctypei[71] = '\0';

      if (ctypei[4] != '-') {
         if (strcmp(ctypei, "CUBEFACE") == 0) {
            if (wcs->cubeface == -1) {
               wcs->cubeface = i;
            } else {
               /* Multiple CUBEFACE axes! */
               return 4;
            }
         }
         continue;
      }


      /* Got an axis qualifier, is it a recognized spectral type? */
      for (k = 0; k < spc_ncode; k++) {
         if (strncmp(ctypei+5, spc_codes[k], 3) == 0) {
            /* Parse the spectral axis type. */
            wcs->spec = i;
            break;
         }
      }

      if (k < spc_ncode) {
         continue;
      }


      /* Is it a recognized celestial projection? */
      for (k = 0; k < prj_ncode; k++) {
         if (strncmp(ctypei+5, prj_codes[k], 3) == 0) break;
      }

      if (k == prj_ncode) {
         /* Maybe it's a projection alias. */
         for (k = 0; k < nalias; k++) {
            if (strncmp(ctypei+5, aliases[k], 3) == 0) break;
         }

         if (k == nalias) {
            /* Not recognized. */
            continue;
         }
      }

      /* Parse the celestial axis type. */
      if (strcmp(pcode, "") == 0) {
         sprintf(pcode, "%.3s", ctypei+5);

         if (strncmp(ctypei, "RA--", 4) == 0) {
            wcs->lng = i;
            strcpy(wcs->lngtyp, "RA");
            strcpy(wcs->lattyp, "DEC");
            ndx = &wcs->lat;
            sprintf(requir, "DEC--%s", pcode);
         } else if (strncmp(ctypei, "DEC-", 4) == 0) {
            wcs->lat = i;
            strcpy(wcs->lngtyp, "RA");
            strcpy(wcs->lattyp, "DEC");
            ndx = &wcs->lng;
            sprintf(requir, "RA---%s", pcode);
         } else if (strncmp(ctypei+1, "LON", 3) == 0) {
            wcs->lng = i;
            sprintf(wcs->lngtyp, "%cLON", ctypei[0]);
            sprintf(wcs->lattyp, "%cLAT", ctypei[0]);
            ndx = &wcs->lat;
            sprintf(requir, "%s-%s", wcs->lattyp, pcode);
         } else if (strncmp(ctypei+1, "LAT", 3) == 0) {
            wcs->lat = i;
            sprintf(wcs->lngtyp, "%cLON", ctypei[0]);
            sprintf(wcs->lattyp, "%cLAT", ctypei[0]);
            ndx = &wcs->lng;
            sprintf(requir, "%s-%s", wcs->lngtyp, pcode);
         } else if (strncmp(ctypei+2, "LN", 2) == 0) {
            wcs->lng = i;
            sprintf(wcs->lngtyp, "%c%cLN", ctypei[0], ctypei[1]);
            sprintf(wcs->lattyp, "%c%cLT", ctypei[0], ctypei[1]);
            ndx = &wcs->lat;
            sprintf(requir, "%s-%s", wcs->lattyp, pcode);
         } else if (strncmp(ctypei+2, "LT", 2) == 0) {
            wcs->lat = i;
            sprintf(wcs->lngtyp, "%c%cLN", ctypei[0], ctypei[1]);
            sprintf(wcs->lattyp, "%c%cLT", ctypei[0], ctypei[1]);
            ndx = &wcs->lng;
            sprintf(requir, "%s-%s", wcs->lngtyp, pcode);
         } else {
            /* Unrecognized celestial type. */
            wcs->lng = -1;
            wcs->lat = -1;
            return 4;
         }
      } else {
         if (strncmp(ctypei, requir, 8) != 0) {
            /* Inconsistent projection types. */
            wcs->lng = -1;
            wcs->lat = -1;
            return 4;
         }

         *ndx = i;
         strcpy(requir, "");
      }
   }


   if (strcmp(requir, "")) {
      /* Unmatched celestial axis. */
      wcs->lng = -1;
      wcs->lat = -1;
      return 4;
   }


   /* Celestial projection present? */
   if (wcs->lng >= 0) {
      celini(wcscel);

      /* CRVALi, LONPOLE, and LATPOLE cards. */
      wcscel->ref[0] = wcs->crval[wcs->lng];
      wcscel->ref[1] = wcs->crval[wcs->lat];
      wcscel->ref[2] = wcs->lonpole;
      wcscel->ref[3] = wcs->latpole;

      /* PVi_m cards. */
      for (k = 0; k < wcs->npv; k++) {
         i = wcs->pv[k].i - 1;
         m = wcs->pv[k].m;

         if (i == wcs->lat) {
            /* PVi_m associated with latitude axis. */
            if (m < 30) {
               wcsprj->pv[m] = wcs->pv[k].value;
            }

         } else if (i == wcs->lng) {
            /* PVi_m associated with longitude axis. */
            switch (m) {
            case 0:
               wcscel->offset = (wcs->pv[k].value != 0.0);
               break;
            case 1:
               wcscel->phi0   = wcs->pv[k].value;
               break;
            case 2:
               wcscel->theta0 = wcs->pv[k].value;
               break;
            case 3:
               /* If present, overrides the LONPOLE card. */
               wcscel->ref[2] = wcs->pv[k].value;
               break;
            case 4:
               /* If present, overrides the LATPOLE card. */
               wcscel->ref[3] = wcs->pv[k].value;
               break;
            default:
               return 6;
               break;
            }
         }
      }

      /* Do simple alias translations. */
      if (strncmp(pcode, "GLS", 3) == 0) {
         strcpy(wcsprj->code, "SFL");

      } else if (strcmp(pcode, "NCP") == 0) {
         /* Convert NCP to SIN. */
         if (wcscel->ref[1] == 0.0) {
            return 5;
         }

         strcpy(wcsprj->code, "SIN");
         wcsprj->pv[1] = 0.0;
         wcsprj->pv[2] = cosd(wcscel->ref[1])/sind(wcscel->ref[1]);

      } else {
         strcpy(wcsprj->code, pcode);
      }

      /* Initialize the celestial transformation routines. */
      wcsprj->r0 = 0.0;
      if (status = celset(wcscel)) {
         return status + 3;
      }
   }


   /* Spectral axis present? */
   if (wcs->spec >= 0) {
      spcini(wcsspc);

      sprintf(wcsspc->type, "%.4s", wcs->ctype[wcs->spec]);
      sprintf(wcsspc->code, "%.3s", wcs->ctype[wcs->spec]+5);

      /* CRVALi, RESTFRQ, and RESTWAV cards. */
      wcsspc->crval = wcs->crval[wcs->spec];
      wcsspc->restfrq = wcs->restfrq;
      wcsspc->restwav = wcs->restwav;

      /* PVi_m cards. */
      for (k = 0; k < wcs->npv; k++) {
         i = wcs->pv[k].i - 1;
         m = wcs->pv[k].m;

         if (i == wcs->spec) {
            /* PVi_m associated with grism axis. */
            if (m < 7) {
               wcsspc->pv[m] = wcs->pv[k].value;
            }
         }
      }

      /* Initialize the spectral transformation routines. */
      if (status = spcset(wcsspc)) {
         return status + 3;
      }
   }


   /* Initialize the linear transformation. */
   wcs->altlin &= 6;
   if (wcs->altlin) {
      pc = wcs->pc;

      if (wcs->altlin & 2) {
         /* Copy CDi_j to PCi_j and reset CDELTi. */
         cd = wcs->cd;
         for (i = 0; i < naxis; i++) {
            for (j = 0; j < naxis; j++) {
               *(pc++) = *(cd++);
            }
            wcs->cdelt[i] = 1.0;
         }

      } else if (wcs->altlin & 4) {
         /* Construct PCi_j from CROTAi. */
         i = wcs->lng;
         j = wcs->lat;
         rho = wcs->crota[j];

         if (wcs->cdelt[i] == 0.0) return 3;
         lambda = wcs->cdelt[j]/wcs->cdelt[i];

         *(pc + i*naxis + i) = *(pc + j*naxis + j) = cosd(rho);
         *(pc + i*naxis + j) = *(pc + j*naxis + i) = sind(rho);
         *(pc + i*naxis + j) *= -lambda;
         *(pc + j*naxis + i) /=  lambda;
      }
   }

   if (status = linset(&(wcs->lin))) {
      return status;
   }

   wcs->flag = WCSSET;

   return 0;
}

/*--------------------------------------------------------------------------*/

int wcsp2s(wcs, ncoord, nelem, pixcrd, imgcrd, phi, theta, world, stat)

struct wcsprm *wcs;
int ncoord, nelem;
const double pixcrd[];
double imgcrd[];
double phi[], theta[];
double world[];
int stat[];

{
   int    face, i, iso_x, iso_y, k, nx, ny, *statp, status, statx2s, wcslat,
          wcslng, wcspec;
   double offset, *worldlat, *worldlng;
   register double *img, *wrl;
   struct celprm *wcscel = &(wcs->cel);

   /* Initialize if required. */
   if (wcs == 0) return 1;
   if (wcs->flag != WCSSET) {
      if (status = wcsset(wcs)) return status;
   }


   /* Apply pixel-to-world linear transformation. */
   if (status = linp2x(&(wcs->lin), ncoord, nelem, pixcrd, imgcrd)) {
      return status;
   }

   wcslng = wcs->lng;
   wcslat = wcs->lat;
   wcspec = wcs->spec;


   /* Convert intermediate world coordinates to world coordinates. */
   img = (double *)imgcrd;
   wrl = (double *)world;
   for (k = 0; k < ncoord; k++) {
      for (i = 0; i < wcs->naxis; i++, img++, wrl++) {
         if (i == wcslng || i == wcslat || i == wcspec) continue;
         *wrl = *img + wcs->crval[i];
      }

      /* Zero the unused world coordinate elements. */
      for (i = wcs->naxis; i < nelem; i++) {
         *(wrl++) = 0.0;
      }

      img += (nelem - wcs->naxis);
   }

   /* Convert celestial coordinates. */
   if (wcs->lng >= 0) {
      /* Do we have a CUBEFACE axis? */
      if (wcs->cubeface != -1) {
         /* Separation between faces. */
         if (wcscel->prj.r0 == 0.0) {
            offset = 90.0;
         } else {
            offset = wcscel->prj.r0*PI/2.0;
         }

         /* Lay out faces in a plane. */
         img = (double *)imgcrd;
         statp = (int *)stat;
         for (k = 0; k < ncoord; k++, statp++) {
            face = (int)(*(img+wcs->cubeface) + 0.5);
            if (fabs(*(img+wcs->cubeface) - face) > 1e-10) {
               *statp = 1;
               status = 8;
            } else {
               switch (face) {
               case 0:
                  *(img+wcslat) += offset;
                  break;
               case 1:
                  break;
               case 2:
                  *(img+wcslng) += offset;
                  break;
               case 3:
                  *(img+wcslng) += offset*2;
                  break;
               case 4:
                  *(img+wcslng) += offset*3;
                  break;
               case 5:
                  *(img+wcslat) -= offset;
                  break;
               default:
                  *statp = 1;
                  status = 8;
               }
            }

            img += nelem;
         }
      }


      /* Check for constant x and/or y. */
      nx = ncoord;
      ny = 0;

      if (iso_x = wcs_allEq(ncoord, nelem, imgcrd+wcslng)) {
         nx = 1;
         ny = ncoord;
      }
      if (iso_y = wcs_allEq(ncoord, nelem, imgcrd+wcslat)) {
         ny = 1;
      }

      /* Transform projection plane coordinates to celestial coordinates. */
      worldlng = (double *)world + wcslng;
      worldlat = (double *)world + wcslat;
      if (statx2s = celx2s(wcscel, nx, ny, nelem, nelem, imgcrd+wcslng,
                           imgcrd+wcslat, phi, theta, worldlng, worldlat,
                           stat)) {;
         if (statx2s == 5) {
            status = 8;
         } else {
            return statx2s + 3;
         }
      }

      /* If x and y were both constant, replicate values. */
      if (iso_x && iso_y) {
         wcs_setAll(ncoord, nelem, worldlng);
         wcs_setAll(ncoord, nelem, worldlat);
         wcs_setAll(ncoord, 1, phi);
         wcs_setAll(ncoord, 1, theta);
         wcs_setAli(ncoord, 1, stat);
      }
   }


   /* Convert spectral coordinates. */
   if (wcs->spec >= 0) {
      /* Check for constant x. */
      nx = ncoord;
      if (iso_x = wcs_allEq(ncoord, nelem, imgcrd+wcspec)) {
         nx = 1;
      }

      if (statx2s = spcx2s(&(wcs->spc), nx, nelem, nelem, imgcrd+wcspec,
                       world+wcspec, stat)) {
         if (statx2s == 3) {
            status = 8;
         } else {
            return statx2s + 3;
         }
      }

      /* If x was constant, replicate values. */
      if (iso_x) {
         wcs_setAll(ncoord, nelem, world+wcspec);
         wcs_setAli(ncoord, 1, stat);
      }
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int wcss2p(wcs, ncoord, nelem, world, phi, theta, imgcrd, pixcrd, stat)

struct wcsprm* wcs;
int ncoord, nelem;
const double world[];
double phi[], theta[];
double imgcrd[];
double pixcrd[];
int stat[];

{
   int    i, isolat, isolng, isospec, k, nlat, nlng, nspec, status, stats2x,
          wcslat, wcslng, wcspec;
   double offset;
   register double *img, *wrl;
   struct celprm *wcscel = &(wcs->cel);


   /* Initialize if required. */
   if (wcs == 0) return 1;
   if (wcs->flag != WCSSET) {
      if (status = wcsset(wcs)) return status;
   }

   wcslng = wcs->lng;
   wcslat = wcs->lat;
   wcspec = wcs->spec;


   /* Convert world coordinates to intermediate world coordinates. */
   wrl = (double *)world;
   img = (double *)imgcrd;

   for (k = 0; k < ncoord; k++) {
      for (i = 0; i < wcs->naxis; i++, wrl++, img++) {
         if (i == wcslng || i == wcslat || i == wcspec) continue;
         *img = *wrl - wcs->crval[i];
      }

      /* Zero the unused intermediate world coordinate elements. */
      for (i = wcs->naxis; i < nelem; i++) {
         *(img++) = 0.0;
      }

      wrl += (nelem - wcs->naxis);
   }


   /* Celestial coordinates. */
   if (wcs->lng >= 0) {
      /* Check for constant lng and/or lat. */
      nlng = ncoord;
      nlat = 0;

      if (isolng = wcs_allEq(ncoord, nelem, world+wcslng)) {
         nlng = 1;
         nlat = ncoord;
      }
      if (isolat = wcs_allEq(ncoord, nelem, world+wcslat)) {
         nlat = 1;
      }

      /* Transform celestial coordinates to projection plane coordinates. */
      if (stats2x = cels2x(wcscel, nlng, nlat, nelem, nelem, world+wcslng,
                           world+wcslat, phi, theta, imgcrd+wcslng,
                           imgcrd+wcslat, stat)) {
         if (stats2x == 6) {
            status = 9;
         } else {
            return stats2x + 3;
         }
      }

      /* If lng and lat were both constant, replicate values. */
      if (isolng && isolat) {
         wcs_setAll(ncoord, nelem, imgcrd+wcslng);
         wcs_setAll(ncoord, nelem, imgcrd+wcslat);
         wcs_setAll(ncoord, 1, phi);
         wcs_setAll(ncoord, 1, theta);
         wcs_setAli(ncoord, 1, stat);
      }

      /* Do we have a CUBEFACE axis? */
      if (wcs->cubeface != -1) {
         /* Separation between faces. */
         if (wcscel->prj.r0 == 0.0) {
            offset = 90.0;
         } else {
            offset = wcscel->prj.r0*PI/2.0;
         }

         /* Stack faces in a cube. */
         img = (double *)imgcrd;
         for (k = 0; k < ncoord; k++) {
            if (*(img+wcslat) < -0.5*offset) {
               *(img+wcslat) += offset;
               *(img+wcs->cubeface) = 5.0;
            } else if (*(img+wcslat) > 0.5*offset) {
               *(img+wcslat) -= offset;
               *(img+wcs->cubeface) = 0.0;
            } else if (*(img+wcslng) > 2.5*offset) {
               *(img+wcslng) -= 3.0*offset;
               *(img+wcs->cubeface) = 4.0;
            } else if (*(img+wcslng) > 1.5*offset) {
               *(img+wcslng) -= 2.0*offset;
               *(img+wcs->cubeface) = 3.0;
            } else if (*(img+wcslng) > 0.5*offset) {
               *(img+wcslng) -= offset;
               *(img+wcs->cubeface) = 2.0;
            } else {
               *(img+wcs->cubeface) = 1.0;
            }

            img += nelem;
         }
      }
   }


   /* Spectral coordinates. */
   if (wcs->spec >= 0) {
      /* Check for constant spec. */
      nspec = ncoord;
      if (isospec = wcs_allEq(ncoord, nelem, world+wcspec)) {
         nspec = 1;
      }

      if (stats2x = spcs2x(&(wcs->spc), nspec, nelem, nelem, world+wcspec,
                       imgcrd+wcspec, stat)) {
         if (stats2x == 4) {
            status = 9;
         } else {
            return stats2x + 3;
         }
      }

      /* If spec was constant, replicate values. */
      if (isospec) {
         wcs_setAll(ncoord, nelem, imgcrd+wcspec);
         wcs_setAli(ncoord, 1, stat);
      }
   }


   /* Apply world-to-pixel linear transformation. */
   if (status = linx2p(&(wcs->lin), ncoord, nelem, imgcrd, pixcrd)) {
      return status;
   }

   return status;
}

/*--------------------------------------------------------------------------*/

int wcsmix(wcs, mixpix, mixcel, vspan, vstep, viter, world, phi, theta,
           imgcrd, pixcrd)

struct wcsprm *wcs;
int mixpix, mixcel;
const double vspan[2];
double vstep;
int viter;
double world[];
double phi[], theta[];
double imgcrd[];
double pixcrd[];

{
   const int niter = 60;
   int    crossed, istep, iter, j, k, nstep, retry, stat[1], status;
   const double tol  = 1.0e-10;
   const double tol2 = 100.0*tol;
   double *worldlat, *worldlng;
   double lambda, span[2], step;
   double pixmix;
   double dlng, lng, lng0, lng0m, lng1, lng1m;
   double dlat, lat, lat0, lat0m, lat1, lat1m;
   double d, d0, d0m, d1, d1m, dx = 0.0;
   double dabs, dmin, lmin;
   double dphi, phi0, phi1;
   struct wcsprm wcs0;

   /* Initialize if required. */
   if (wcs == 0) return 1;
   if (wcs->flag != WCSSET) {
      if (status = wcsset(wcs)) return status;
   }

   worldlng = world + wcs->lng;
   worldlat = world + wcs->lat;


   /* Check vspan. */
   if (vspan[0] <= vspan[1]) {
      span[0] = vspan[0];
      span[1] = vspan[1];
   } else {
      /* Swap them. */
      span[0] = vspan[1];
      span[1] = vspan[0];
   }

   /* Check vstep. */
   step = fabs(vstep);
   if (step == 0.0) {
      step = (span[1] - span[0])/10.0;
      if (step > 1.0 || step == 0.0) step = 1.0;
   }

   /* Check viter. */
   nstep = viter;
   if (nstep < 5) {
      nstep = 5;
   } else if (nstep > 10) {
      nstep = 10;
   }

   /* Given pixel element. */
   pixmix = pixcrd[mixpix];

   /* Iterate on the step size. */
   for (istep = 0; istep <= nstep; istep++) {
      if (istep) step /= 2.0;

      /* Iterate on the sky coordinate between the specified range. */
      if (mixcel == 1) {
         /* Celestial longitude is given. */

         /* Check whether the solution interval is a crossing interval. */
         lat0 = span[0];
         *worldlat = lat0;
         if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }
         d0 = pixcrd[mixpix] - pixmix;

         dabs = fabs(d0);
         if (dabs < tol) return 0;

         lat1 = span[1];
         *worldlat = lat1;
         if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }
         d1 = pixcrd[mixpix] - pixmix;

         dabs = fabs(d1);
         if (dabs < tol) return 0;

         lmin = lat1;
         dmin = dabs;

         /* Check for a crossing point. */
         if (signbit(d0) != signbit(d1)) {
            crossed = 1;
            dx = d1;
         } else {
            crossed = 0;
            lat0 = span[1];
         }

         for (retry = 0; retry < 4; retry++) {
            /* Refine the solution interval. */
            while (lat0 > span[0]) {
               lat0 -= step;
               if (lat0 < span[0]) lat0 = span[0];
               *worldlat = lat0;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                   pixcrd, stat)) {
                  return (status == 9) ? 10 : status;
               }
               d0 = pixcrd[mixpix] - pixmix;

               /* Check for a solution. */
               dabs = fabs(d0);
               if (dabs < tol) return 0;

               /* Record the point of closest approach. */
               if (dabs < dmin) {
                  lmin = lat0;
                  dmin = dabs;
               }

               /* Check for a crossing point. */
               if (signbit(d0) != signbit(d1)) {
                  crossed = 2;
                  dx = d0;
                  break;
               }

               /* Advance to the next subinterval. */
               lat1 = lat0;
               d1 = d0;
            }

            if (crossed) {
               /* A crossing point was found. */
               for (iter = 0; iter < niter; iter++) {
                  /* Use regula falsi division of the interval. */
                  lambda = d0/(d0-d1);
                  if (lambda < 0.1) {
                     lambda = 0.1;
                  } else if (lambda > 0.9) {
                     lambda = 0.9;
                  }

                  dlat = lat1 - lat0;
                  lat = lat0 + lambda*dlat;
                  *worldlat = lat;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }

                  /* Check for a solution. */
                  d = pixcrd[mixpix] - pixmix;
                  dabs = fabs(d);
                  if (dabs < tol) return 0;

                  if (dlat < tol) {
                     /* An artifact of numerical imprecision. */
                     if (dabs < tol2) return 0;

                     /* Must be a discontinuity. */
                     break;
                  }

                  /* Record the point of closest approach. */
                  if (dabs < dmin) {
                     lmin = lat;
                     dmin = dabs;
                  }

                  if (signbit(d0) == signbit(d)) {
                     lat0 = lat;
                     d0 = d;
                  } else {
                     lat1 = lat;
                     d1 = d;
                  }
               }

               /* No convergence, must have been a discontinuity. */
               if (crossed == 1) lat0 = span[1];
               lat1 = lat0;
               d1 = dx;
               crossed = 0;

            } else {
               /* No crossing point; look for a tangent point. */
               if (lmin == span[0]) break;
               if (lmin == span[1]) break;

               lat = lmin;
               lat0 = lat - step;
               if (lat0 < span[0]) lat0 = span[0];
               lat1 = lat + step;
               if (lat1 > span[1]) lat1 = span[1];

               *worldlat = lat0;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                   pixcrd, stat)) {
                  return (status == 9) ? 10 : status;
               }
               d0 = fabs(pixcrd[mixpix] - pixmix);

               d  = dmin;

               *worldlat = lat1;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                   pixcrd, stat)) {
                  return (status == 9) ? 10 : status;
               }
               d1 = fabs(pixcrd[mixpix] - pixmix);

               for (iter = 0; iter < niter; iter++) {
                  lat0m = (lat0 + lat)/2.0;
                  *worldlat = lat0m;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }
                  d0m = fabs(pixcrd[mixpix] - pixmix);

                  if (d0m < tol) return 0;

                  lat1m = (lat1 + lat)/2.0;
                  *worldlat = lat1m;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }
                  d1m = fabs(pixcrd[mixpix] - pixmix);

                  if (d1m < tol) return 0;

                  if (d0m < d && d0m <= d1m) {
                     lat1 = lat;
                     d1   = d;
                     lat  = lat0m;
                     d    = d0m;
                  } else if (d1m < d) {
                     lat0 = lat;
                     d0   = d;
                     lat  = lat1m;
                     d    = d1m;
                  } else {
                     lat0 = lat0m;
                     d0   = d0m;
                     lat1 = lat1m;
                     d1   = d1m;
                  }
               }
            }
         }

      } else {
         /* Celestial latitude is given. */

         /* Check whether the solution interval is a crossing interval. */
         lng0 = span[0];
         *worldlng = lng0;
         if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }
         d0 = pixcrd[mixpix] - pixmix;

         dabs = fabs(d0);
         if (dabs < tol) return 0;

         lng1 = span[1];
         *worldlng = lng1;
         if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }
         d1 = pixcrd[mixpix] - pixmix;

         dabs = fabs(d1);
         if (dabs < tol) return 0;
         lmin = lng1;
         dmin = dabs;

         /* Check for a crossing point. */
         if (signbit(d0) != signbit(d1)) {
            crossed = 1;
            dx = d1;
         } else {
            crossed = 0;
            lng0 = span[1];
         }

         for (retry = 0; retry < 4; retry++) {
            /* Refine the solution interval. */
            while (lng0 > span[0]) {
               lng0 -= step;
               if (lng0 < span[0]) lng0 = span[0];
               *worldlng = lng0;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd, pixcrd,
                                   stat)) {
                  return (status == 9) ? 10 : status;
               }
               d0 = pixcrd[mixpix] - pixmix;

               /* Check for a solution. */
               dabs = fabs(d0);
               if (dabs < tol) return 0;

               /* Record the point of closest approach. */
               if (dabs < dmin) {
                  lmin = lng0;
                  dmin = dabs;
               }

               /* Check for a crossing point. */
               if (signbit(d0) != signbit(d1)) {
                  crossed = 2;
                  dx = d0;
                  break;
               }

               /* Advance to the next subinterval. */
               lng1 = lng0;
               d1 = d0;
            }

            if (crossed) {
               /* A crossing point was found. */
               for (iter = 0; iter < niter; iter++) {
                  /* Use regula falsi division of the interval. */
                  lambda = d0/(d0-d1);
                  if (lambda < 0.1) {
                     lambda = 0.1;
                  } else if (lambda > 0.9) {
                     lambda = 0.9;
                  }

                  dlng = lng1 - lng0;
                  lng = lng0 + lambda*dlng;
                  *worldlng = lng;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }

                  /* Check for a solution. */
                  d = pixcrd[mixpix] - pixmix;
                  dabs = fabs(d);
                  if (dabs < tol) return 0;

                  if (dlng < tol) {
                     /* An artifact of numerical imprecision. */
                     if (dabs < tol2) return 0;

                     /* Must be a discontinuity. */
                     break;
                  }

                  /* Record the point of closest approach. */
                  if (dabs < dmin) {
                     lmin = lng;
                     dmin = dabs;
                  }

                  if (signbit(d0) == signbit(d)) {
                     lng0 = lng;
                     d0 = d;
                  } else {
                     lng1 = lng;
                     d1 = d;
                  }
               }

               /* No convergence, must have been a discontinuity. */
               if (crossed == 1) lng0 = span[1];
               lng1 = lng0;
               d1 = dx;
               crossed = 0;

            } else {
               /* No crossing point; look for a tangent point. */
               if (lmin == span[0]) break;
               if (lmin == span[1]) break;

               lng = lmin;
               lng0 = lng - step;
               if (lng0 < span[0]) lng0 = span[0];
               lng1 = lng + step;
               if (lng1 > span[1]) lng1 = span[1];

               *worldlng = lng0;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                   pixcrd, stat)) {
                  return (status == 9) ? 10 : status;
               }
               d0 = fabs(pixcrd[mixpix] - pixmix);

               d  = dmin;

               *worldlng = lng1;
               if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                   pixcrd, stat)) {
                  return (status == 9) ? 10 : status;
               }
               d1 = fabs(pixcrd[mixpix] - pixmix);

               for (iter = 0; iter < niter; iter++) {
                  lng0m = (lng0 + lng)/2.0;
                  *worldlng = lng0m;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }
                  d0m = fabs(pixcrd[mixpix] - pixmix);

                  if (d0m < tol) return 0;

                  lng1m = (lng1 + lng)/2.0;
                  *worldlng = lng1m;
                  if (status = wcss2p(wcs, 1, 0, world, phi, theta, imgcrd,
                                      pixcrd, stat)) {
                     return (status == 9) ? 10 : status;
                  }
                  d1m = fabs(pixcrd[mixpix] - pixmix);

                  if (d1m < tol) return 0;

                  if (d0m < d && d0m <= d1m) {
                     lng1 = lng;
                     d1   = d;
                     lng  = lng0m;
                     d    = d0m;
                  } else if (d1m < d) {
                     lng0 = lng;
                     d0   = d;
                     lng  = lng1m;
                     d    = d1m;
                  } else {
                     lng0 = lng0m;
                     d0   = d0m;
                     lng1 = lng1m;
                     d1   = d1m;
                  }
               }
            }
         }
      }
   }


   /* Set cel0 to the unity transformation. */
   wcs0 = *wcs;
   wcs0.cel.euler[0] = -90.0;
   wcs0.cel.euler[1] =   0.0;
   wcs0.cel.euler[2] =  90.0;
   wcs0.cel.euler[3] =   1.0;
   wcs0.cel.euler[4] =   0.0;

   /* No convergence, check for aberrant behaviour at a native pole. */
   *theta = -90.0;
   for (j = 1; j <= 2; j++) {
      /* Could the celestial coordinate element map to a native pole? */
      *phi = 0.0;
      *theta = -*theta;
      status = sphx2s(wcs->cel.euler, 1, 1, 1, 1, phi, theta, &lng, &lat);

      if (mixcel == 1) {
         if (fabs(fmod(*worldlng-lng, 360.0)) > tol) continue;
         if (lat < span[0]) continue;
         if (lat > span[1]) continue;
         *worldlat = lat;
      } else {
         if (fabs(*worldlat-lat) > tol) continue;
         if (lng < span[0]) lng += 360.0;
         if (lng > span[1]) lng -= 360.0;
         if (lng < span[0]) continue;
         if (lng > span[1]) continue;
         *worldlng = lng;
      }

      /* Is there a solution for the given pixel coordinate element? */
      lng = *worldlng;
      lat = *worldlat;

      /* Feed native coordinates to wcss2p() with cel0 set to unity. */
      *worldlng = -180.0;
      *worldlat = *theta;
      if (status = wcss2p(&wcs0, 1, 0, world, phi, theta, imgcrd, pixcrd,
                          stat)) {
         return (status == 9) ? 10 : status;
      }
      d0 = pixcrd[mixpix] - pixmix;

      /* Check for a solution. */
      if (fabs(d0) < tol) {
         /* Recall saved world coordinates. */
         *worldlng = lng;
         *worldlat = lat;
         return 0;
      }

      /* Search for a crossing interval. */
      phi0 = -180.0;
      for (k = -179; k <= 180; k++) {
         phi1 = (double) k;
         *worldlng = phi1;
         if (status = wcss2p(&wcs0, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }
         d1 = pixcrd[mixpix] - pixmix;

         /* Check for a solution. */
         dabs = fabs(d1);
         if (dabs < tol) {
            /* Recall saved world coordinates. */
            *worldlng = lng;
            *worldlat = lat;
            return 0;
         }

         /* Is it a crossing interval? */
         if (signbit(d0) != signbit(d1)) break;

         phi0 = phi1;
         d0 = d1;
      }

      for (iter = 1; iter <= niter; iter++) {
         /* Use regula falsi division of the interval. */
         lambda = d0/(d0-d1);
         if (lambda < 0.1) {
            lambda = 0.1;
         } else if (lambda > 0.9) {
            lambda = 0.9;
         }

         dphi = phi1 - phi0;
         *worldlng = phi0 + lambda*dphi;
         if (status = wcss2p(&wcs0, 1, 0, world, phi, theta, imgcrd, pixcrd,
                             stat)) {
            return (status == 9) ? 10 : status;
         }

         /* Check for a solution. */
         d = pixcrd[mixpix] - pixmix;
         dabs = fabs(d);
         if (dabs < tol || (dphi < tol && dabs < tol2)) {
            /* Recall saved world coordinates. */
            *worldlng = lng;
            *worldlat = lat;
            return 0;
         }

         if (signbit(d0) == signbit(d)) {
            phi0 = *worldlng;
            d0 = d;
         } else {
            phi1 = *worldlng;
            d1 = d;
         }
      }
   }


   /* No solution. */
   return 11;
}

/*--------------------------------------------------------------------------*/

int wcs_allEq(ncoord, nelem, first)

int ncoord, nelem;
const double *first;

{
   double v0;
   const double *vp;

   v0 = *first;
   for (vp = first+nelem; vp < first + ncoord*nelem; vp += nelem) {
     if (*vp != v0) return 0;
   }

   return 1;
}

/*--------------------------------------------------------------------------*/

void wcs_setAll(ncoord, nelem, first)

int ncoord, nelem;
double *first;

{
   double v0, *vp;

   v0 = *first;
   for (vp = first+nelem; vp < first + ncoord*nelem; vp += nelem) {
     *vp = v0;
   }
}

/*--------------------------------------------------------------------------*/

void wcs_setAli(ncoord, nelem, first)

int ncoord, nelem;
int *first;

{
   int v0, *vp;

   v0 = *first;
   for (vp = first+nelem; vp < first + ncoord*nelem; vp += nelem) {
     *vp = v0;
   }
}
