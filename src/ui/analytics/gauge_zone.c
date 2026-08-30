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
UmiStatus umi_analytics_gauge_zone_init(UmiAnalyticsGaugeZone *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->minimum=0.0;item->maximum=1.0;item->severity=UMI_ANALYTICS_SEVERITY_INFO;return UMI_STATUS_OK;}
int umi_analytics_gauge_zone_valid(const UmiAnalyticsGaugeZone *item){if(item==NULL)return 0;return (item->minimum<item->maximum&&item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}
