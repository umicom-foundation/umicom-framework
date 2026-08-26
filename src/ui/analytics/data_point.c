/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/data_point.c
 *
 * PURPOSE:
 *   Represent one finite Cartesian chart sample.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/data_point.h"

#include <math.h>
UmiStatus umi_analytics_data_point_init(UmiAnalyticsDataPoint *point, double x, double y) {
    if (point == NULL || !isfinite(x) || !isfinite(y)) return UMI_STATUS_INVALID_ARGUMENT;
    point->x = x; point->y = y; point->valid = 1; return UMI_STATUS_OK;
}
int umi_analytics_data_point_is_valid(const UmiAnalyticsDataPoint *point) {
    return point != NULL && point->valid != 0 && isfinite(point->x) && isfinite(point->y);
}
