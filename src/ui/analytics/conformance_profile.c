/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/conformance_profile.c
 *
 * PURPOSE:
 *   Declare renderer conformance requirements for shared analytics surfaces.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/conformance_profile.h"

#include <string.h>
void umi_analytics_conformance_profile_required(UmiAnalyticsConformanceProfile *p){if(p!=NULL){memset(p,0,sizeof *p);p->charts=1;p->gauges=1;p->heatmaps=1;p->dashboards=1;p->keyboard_access=1;p->high_dpi=1;p->export_text=1;}}
int umi_analytics_conformance_profile_satisfied(const UmiAnalyticsConformanceProfile *r,const UmiAnalyticsConformanceProfile *a){if(r==NULL||a==NULL)return 0;return (!r->charts||a->charts)&&(!r->gauges||a->gauges)&&(!r->heatmaps||a->heatmaps)&&(!r->dashboards||a->dashboards)&&(!r->keyboard_access||a->keyboard_access)&&(!r->high_dpi||a->high_dpi)&&(!r->export_text||a->export_text);}
