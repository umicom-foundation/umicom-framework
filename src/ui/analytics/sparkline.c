/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/sparkline.c
 *
 * PURPOSE:
 *   Summarize compact time-series direction and range for dashboard cells.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/sparkline.h"

#include <math.h>
UmiStatus umi_analytics_sparkline_summarize(const double *values,size_t count,UmiAnalyticsSparklineSummary *out_summary){size_t i,seen=0U;if(values==NULL||out_summary==NULL)return UMI_STATUS_INVALID_ARGUMENT;for(i=0;i<count;i++)if(isfinite(values[i])){if(seen==0U){out_summary->first=out_summary->minimum=out_summary->maximum=values[i];}else{if(values[i]<out_summary->minimum)out_summary->minimum=values[i];if(values[i]>out_summary->maximum)out_summary->maximum=values[i];}out_summary->last=values[i];seen++;}if(seen==0U)return UMI_STATUS_NOT_FOUND;out_summary->count=seen;out_summary->change=out_summary->last-out_summary->first;return UMI_STATUS_OK;}
