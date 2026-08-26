/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/axis_scale.c
 *
 * PURPOSE:
 *   Map axis-domain values into normalized presentation coordinates.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/axis_scale.h"

#include <math.h>
UmiStatus umi_analytics_axis_scale_normalize(const UmiAnalyticsAxis *axis,double value,double *out_normalized){double lo,hi,v;if(!umi_analytics_axis_valid(axis)||out_normalized==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;if(axis->scale==UMI_ANALYTICS_SCALE_LOG10){if(axis->minimum<=0.0||value<=0.0)return UMI_STATUS_INVALID_ARGUMENT;lo=log10(axis->minimum);hi=log10(axis->maximum);v=log10(value);}else{lo=axis->minimum;hi=axis->maximum;v=value;}*out_normalized=(v-lo)/(hi-lo);return UMI_STATUS_OK;}
