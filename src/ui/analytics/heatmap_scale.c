/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/heatmap_scale.c
 *
 * PURPOSE:
 *   Normalize sequential and diverging heatmap values.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/heatmap_scale.h"

/*
 * Initialise analytics heatmap scale from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_heatmap_scale_init(UmiAnalyticsHeatmapScale *s,double minimum,double center,double maximum,int diverging){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||minimum>=maximum||center<minimum||center>maximum)return UMI_STATUS_INVALID_ARGUMENT;s->minimum=minimum;s->center=center;s->maximum=maximum;s->diverging=diverging?1:0;return UMI_STATUS_OK;}
/*
 * Provide the analytics heatmap scale normalize operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_heatmap_scale_normalize(const UmiAnalyticsHeatmapScale *s,double value,double *out_normalized){double v;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s==NULL||out_normalized==NULL)return UMI_STATUS_INVALID_ARGUMENT;v=value;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<s->minimum)v=s->minimum;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v>s->maximum)v=s->maximum;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(s->diverging){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(v<=s->center){double span=s->center-s->minimum;*out_normalized=span>0.0?0.5*(v-s->minimum)/span:0.5;}/* Use this fallback path when the earlier condition does not apply. */ else{double span=s->maximum-s->center;*out_normalized=span>0.0?0.5+0.5*(v-s->center)/span:0.5;}}/* Use this fallback path when the earlier condition does not apply. */ else *out_normalized=(v-s->minimum)/(s->maximum-s->minimum);return UMI_STATUS_OK;}
