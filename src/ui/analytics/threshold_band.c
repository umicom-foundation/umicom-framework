/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/threshold_band.c
 *
 * PURPOSE:
 *   Describe semantic threshold bands for risk, limits and operational analytics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/threshold_band.h"

#include <string.h>
/*
 * Initialise analytics threshold band from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_threshold_band_init(UmiAnalyticsThresholdBand *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->lower=0.0;item->upper=1.0;item->severity=UMI_ANALYTICS_SEVERITY_WARNING;return UMI_STATUS_OK;}
/*
 * Check that analytics threshold band satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_threshold_band_valid(const UmiAnalyticsThresholdBand *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->lower<=item->upper&&item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}
