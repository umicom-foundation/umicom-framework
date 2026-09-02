/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis_scale.c
 *
 * PURPOSE:
 *   Map axis-domain values into normalized presentation coordinates.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_scale.h"

#include <math.h>
/*
 * Provide the analytics axis scale normalize operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_axis_scale_normalize(const UmiAnalyticsAxis *axis,double value,double *out_normalized){double lo,hi,v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_analytics_axis_valid(axis)||out_normalized==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(axis->scale==UMI_ANALYTICS_SCALE_LOG10){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(axis->minimum<=0.0||value<=0.0)return UMI_STATUS_INVALID_ARGUMENT;lo=log10(axis->minimum);hi=log10(axis->maximum);v=log10(value);}/* Use this fallback path when the earlier condition does not apply. */ else{lo=axis->minimum;hi=axis->maximum;v=value;}*out_normalized=(v-lo)/(hi-lo);return UMI_STATUS_OK;}
