/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/heatmap_scale.c
 *
 * PURPOSE:
 *   Normalize sequential and diverging heatmap values.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_scale.h"

UmiStatus umi_analytics_heatmap_scale_init(UmiAnalyticsHeatmapScale *s,double minimum,double center,double maximum,int diverging){if(s==NULL||minimum>=maximum||center<minimum||center>maximum)return UMI_STATUS_INVALID_ARGUMENT;s->minimum=minimum;s->center=center;s->maximum=maximum;s->diverging=diverging?1:0;return UMI_STATUS_OK;}
UmiStatus umi_analytics_heatmap_scale_normalize(const UmiAnalyticsHeatmapScale *s,double value,double *out_normalized){double v;if(s==NULL||out_normalized==NULL)return UMI_STATUS_INVALID_ARGUMENT;v=value;if(v<s->minimum)v=s->minimum;if(v>s->maximum)v=s->maximum;if(s->diverging){if(v<=s->center){double span=s->center-s->minimum;*out_normalized=span>0.0?0.5*(v-s->minimum)/span:0.5;}else{double span=s->maximum-s->center;*out_normalized=span>0.0?0.5+0.5*(v-s->center)/span:0.5;}}else *out_normalized=(v-s->minimum)/(s->maximum-s->minimum);return UMI_STATUS_OK;}
