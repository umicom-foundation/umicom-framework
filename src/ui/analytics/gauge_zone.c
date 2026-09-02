/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/gauge_zone.c
 *
 * PURPOSE:
 *   Define ordered gauge threshold zones with semantic severity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/gauge_zone.h"

#include <string.h>
/*
 * Initialise analytics gauge zone from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_gauge_zone_init(UmiAnalyticsGaugeZone *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->minimum=0.0;item->maximum=1.0;item->severity=UMI_ANALYTICS_SEVERITY_INFO;return UMI_STATUS_OK;}
/*
 * Check that analytics gauge zone satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_gauge_zone_valid(const UmiAnalyticsGaugeZone *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->minimum<item->maximum&&item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}
