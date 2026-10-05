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
#include "../../base/value_archive_internal.h"

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

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsDataPointArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x56cfd7ae4aa562c9);

    return schema;
}
static size_t UmiAnalyticsDataPointArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsDataPointArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsDataPoint *value)
{
    UmiArchiveWriteDouble(writer, value->x);
    UmiArchiveWriteDouble(writer, value->y);
    UmiArchiveWriteSigned(writer, (int64_t)value->valid);
}
static void UmiAnalyticsDataPointArchiveRead(UmiArchiveReader *reader, UmiAnalyticsDataPoint *value)
{
    value->x = UmiArchiveReadDouble(reader);
    value->y = UmiArchiveReadDouble(reader);
    value->valid = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsDataPointArchiveValidate(const UmiAnalyticsDataPoint *value)
{
    return umi_analytics_data_point_is_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_data_point_archive_encode, umi_analytics_data_point_archive_decode,
    UmiAnalyticsDataPoint, UmiAnalyticsDataPointArchiveSchema, UmiAnalyticsDataPointArchiveBound, UmiAnalyticsDataPointArchiveWrite, UmiAnalyticsDataPointArchiveRead, UmiAnalyticsDataPointArchiveValidate)
