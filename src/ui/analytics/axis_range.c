/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis_range.c
 *
 * PURPOSE:
 *   Expand finite chart ranges and apply proportional padding.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_range.h"

#include <math.h>
#include <string.h>
void umi_analytics_axis_range_reset(UmiAnalyticsAxisRange *range){if(range!=NULL)memset(range,0,sizeof *range);}
UmiStatus umi_analytics_axis_range_include(UmiAnalyticsAxisRange *range,double value){if(range==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;if(!range->initialized){range->minimum=range->maximum=value;range->initialized=1;}else{if(value<range->minimum)range->minimum=value;if(value>range->maximum)range->maximum=value;}return UMI_STATUS_OK;}
UmiStatus umi_analytics_axis_range_padded(const UmiAnalyticsAxisRange *range,double fraction,UmiAnalyticsAxisRange *out_range){double span,pad;if(range==NULL||out_range==NULL||!range->initialized||fraction<0.0)return UMI_STATUS_INVALID_ARGUMENT;span=range->maximum-range->minimum;if(span==0.0)span=fabs(range->minimum)>1.0?fabs(range->minimum):1.0;pad=span*fraction;*out_range=*range;out_range->minimum-=pad;out_range->maximum+=pad;return UMI_STATUS_OK;}
