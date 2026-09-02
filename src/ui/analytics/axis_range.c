/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis_range.c
 *
 * PURPOSE:
 *   Expand finite chart ranges and apply proportional padding.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_range.h"

#include <math.h>
#include <string.h>
/*
 * Release or reset state held by analytics axis range so the same storage can be reused
 * safely.
 */
void umi_analytics_axis_range_reset(UmiAnalyticsAxisRange *range){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(range!=NULL)memset(range,0,sizeof *range);}
/*
 * Provide the analytics axis range include operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_axis_range_include(UmiAnalyticsAxisRange *range,double value){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(range==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!range->initialized){range->minimum=range->maximum=value;range->initialized=1;}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value<range->minimum)range->minimum=value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(value>range->maximum)range->maximum=value;}return UMI_STATUS_OK;}
/*
 * Provide the analytics axis range padded operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_axis_range_padded(const UmiAnalyticsAxisRange *range,double fraction,UmiAnalyticsAxisRange *out_range){double span,pad;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(range==NULL||out_range==NULL||!range->initialized||fraction<0.0)return UMI_STATUS_INVALID_ARGUMENT;span=range->maximum-range->minimum;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(span==0.0)span=fabs(range->minimum)>1.0?fabs(range->minimum):1.0;pad=span*fraction;*out_range=*range;out_range->minimum-=pad;out_range->maximum+=pad;return UMI_STATUS_OK;}
