/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/conformance_profile.c
 *
 * PURPOSE:
 *   Declare renderer conformance requirements for shared analytics surfaces.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/conformance_profile.h"

#include <string.h>
/*
 * Provide the analytics conformance profile required operation used by this module and its
 * client applications.
 */
void umi_analytics_conformance_profile_required(UmiAnalyticsConformanceProfile *p){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(p!=NULL){memset(p,0,sizeof *p);p->charts=1;p->gauges=1;p->heatmaps=1;p->dashboards=1;p->keyboard_access=1;p->high_dpi=1;p->export_text=1;}}
/*
 * Provide the analytics conformance profile satisfied operation used by this module and
 * its client applications.
 */
int umi_analytics_conformance_profile_satisfied(const UmiAnalyticsConformanceProfile *r,const UmiAnalyticsConformanceProfile *a){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(r==NULL||a==NULL)return 0;return (!r->charts||a->charts)&&(!r->gauges||a->gauges)&&(!r->heatmaps||a->heatmaps)&&(!r->dashboards||a->dashboards)&&(!r->keyboard_access||a->keyboard_access)&&(!r->high_dpi||a->high_dpi)&&(!r->export_text||a->export_text);}
