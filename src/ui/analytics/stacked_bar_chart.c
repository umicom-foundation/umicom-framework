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

UmiStatus umi_analytics_stacked_bar_total(const double *values,size_t count,UmiAnalyticsStackTotal *out_total){size_t i;if(values==NULL||out_total==NULL)return UMI_STATUS_INVALID_ARGUMENT;out_total->positive=0.0;out_total->negative=0.0;for(i=0;i<count;i++){if(!umi_analytics_number_valid(values[i]))continue;if(values[i]>=0.0)out_total->positive+=values[i];else out_total->negative+=values[i];}return UMI_STATUS_OK;}
