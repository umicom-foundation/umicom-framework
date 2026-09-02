/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/sparkline.c
 *
 * PURPOSE:
 *   Summarize compact time-series direction and range for dashboard cells.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/sparkline.h"

#include <math.h>
/*
 * Provide the analytics sparkline summarize operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_sparkline_summarize(const double *values,size_t count,UmiAnalyticsSparklineSummary *out_summary){size_t i,seen=0U;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_summary==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(isfinite(values[i])){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U){out_summary->first=out_summary->minimum=out_summary->maximum=values[i];}/* Use this fallback path when the earlier condition does not apply. */ else{/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]<out_summary->minimum)out_summary->minimum=values[i];/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>out_summary->maximum)out_summary->maximum=values[i];}out_summary->last=values[i];seen++;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(seen==0U)return UMI_STATUS_NOT_FOUND;out_summary->count=seen;out_summary->change=out_summary->last-out_summary->first;return UMI_STATUS_OK;}
