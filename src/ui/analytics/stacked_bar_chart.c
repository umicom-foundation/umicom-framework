/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/stacked_bar_chart.c
 *
 * PURPOSE:
 *   Accumulate positive/negative stack totals for stacked bars.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/stacked_bar_chart.h"

/*
 * Provide the analytics stacked bar total operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_stacked_bar_total(const double *values,size_t count,UmiAnalyticsStackTotal *out_total){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values==NULL||out_total==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_total->positive=0.0;out_total->negative=0.0;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<count;i++){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(!umi_analytics_number_valid(values[i]))continue;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(values[i]>=0.0)out_total->positive+=values[i];/* Use this fallback path when the earlier condition does not apply. */ else out_total->negative+=values[i];}return UMI_STATUS_OK;}
