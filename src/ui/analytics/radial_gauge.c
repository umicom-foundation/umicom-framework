/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/radial_gauge.c
 *
 * PURPOSE:
 *   Configure radial-gauge angular sweep and needle visibility.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/radial_gauge.h"

#include <string.h>
/*
 * Initialise analytics radial gauge from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_radial_gauge_init(UmiAnalyticsRadialGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->start_degrees=-135.0;item->sweep_degrees=270.0;item->needle=1;return UMI_STATUS_OK;}
/*
 * Check that analytics radial gauge satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_radial_gauge_valid(const UmiAnalyticsRadialGauge *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->sweep_degrees>0.0&&item->sweep_degrees<=360.0)?1:0;}
