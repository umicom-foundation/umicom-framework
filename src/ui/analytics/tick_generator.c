/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/tick_generator.c
 *
 * PURPOSE:
 *   Generate deterministic evenly spaced linear axis ticks.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/tick_generator.h"

/*
 * Provide the analytics ticks linear operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_ticks_linear(double minimum,double maximum,size_t requested,UmiAnalyticsTicks *out_ticks){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(out_ticks==NULL||requested<2U||requested>UMI_ANALYTICS_MAX_BINS||minimum>=maximum)return UMI_STATUS_INVALID_ARGUMENT;out_ticks->count=requested;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<requested;i++)out_ticks->values[i]=minimum+(maximum-minimum)*(double)i/(double)(requested-1U);return UMI_STATUS_OK;}
