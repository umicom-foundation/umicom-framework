/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/linear_gauge.c
 *
 * PURPOSE:
 *   Configure horizontal/vertical linear gauge presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/linear_gauge.h"

#include <string.h>
UmiStatus umi_analytics_linear_gauge_init(UmiAnalyticsLinearGauge *item){if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->orientation=UMI_ANALYTICS_HORIZONTAL;return UMI_STATUS_OK;}
int umi_analytics_linear_gauge_valid(const UmiAnalyticsLinearGauge *item){if(item==NULL)return 0;return (item->orientation==UMI_ANALYTICS_HORIZONTAL||item->orientation==UMI_ANALYTICS_VERTICAL)?1:0;}
