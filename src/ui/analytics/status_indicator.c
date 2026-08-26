/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/status_indicator.c
 *
 * PURPOSE:
 *   Represent compact semantic status indicators for dashboards and workbenches.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/status_indicator.h"

#include <string.h>
UmiStatus umi_analytics_status_indicator_init(UmiAnalyticsStatusIndicator *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);(void)umi_analytics_copy_text(item->label,sizeof item->label,"Status");item->severity=UMI_ANALYTICS_SEVERITY_INFO;item->active=1;return UMI_STATUS_OK;}
int umi_analytics_status_indicator_valid(const UmiAnalyticsStatusIndicator *item){if(item==NULL)return 0;return (item->severity>=UMI_ANALYTICS_SEVERITY_INFO&&item->severity<=UMI_ANALYTICS_SEVERITY_ERROR)?1:0;}
