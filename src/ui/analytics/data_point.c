/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/data_point.c
 *
 * PURPOSE:
 *   Represent one finite Cartesian chart sample.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_point.h"

#include <math.h>
/*
 * Initialise analytics data point from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_data_point_init(UmiAnalyticsDataPoint *point, double x, double y) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (point == NULL || !isfinite(x) || !isfinite(y)) return UMI_STATUS_INVALID_ARGUMENT;
    point->x = x; point->y = y; point->valid = 1; return UMI_STATUS_OK;
}
/*
 * Check that analytics data point satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_data_point_is_valid(const UmiAnalyticsDataPoint *point) {
    return point != NULL && point->valid != 0 && isfinite(point->x) && isfinite(point->y);
}
