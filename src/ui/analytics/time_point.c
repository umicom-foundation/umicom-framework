/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/time_point.c
 *
 * PURPOSE:
 *   Represent one timestamped analytics sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/time_point.h"

#include <math.h>
/*
 * Initialise analytics time point from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_time_point_init(UmiAnalyticsTimePoint *point, int64_t timestamp_ns, double value){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(point==NULL||!isfinite(value))return UMI_STATUS_INVALID_ARGUMENT;point->timestamp_ns=timestamp_ns;point->value=value;return UMI_STATUS_OK;}
/*
 * Provide the analytics time point compare operation used by this module and its client
 * applications.
 */
int umi_analytics_time_point_compare(const UmiAnalyticsTimePoint *left,const UmiAnalyticsTimePoint *right){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(left==NULL||right==NULL)return 0;return (left->timestamp_ns>right->timestamp_ns)-(left->timestamp_ns<right->timestamp_ns);}
