/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/time_point.c
 *
 * PURPOSE:
 *   Represent one timestamped analytics sample.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_point.h"

#include <math.h>
UmiStatus umi_analytics_time_point_init(UmiAnalyticsTimePoint *point, int64_t timestamp_ns, double value){if(point==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;point->timestamp_ns=timestamp_ns;point->value=value;return UMI_STATUS_OK;}
int umi_analytics_time_point_compare(const UmiAnalyticsTimePoint *left,const UmiAnalyticsTimePoint *right){if(left==NULL||right==NULL)return 0;return (left->timestamp_ns>right->timestamp_ns)-(left->timestamp_ns<right->timestamp_ns);}
