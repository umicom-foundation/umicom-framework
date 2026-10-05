/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/area_chart.c
 *
 * PURPOSE:
 *   Configure area-chart stacking and fill opacity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/area_chart.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics area chart from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_area_chart_init(UmiAnalyticsAreaChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->opacity=0.35;return UMI_STATUS_OK;}
/*
 * Check that analytics area chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_area_chart_valid(const UmiAnalyticsAreaChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->opacity>=0.0&&item->opacity<=1.0)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsAreaChartArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xeccd25d733280d7b);

    return schema;
}
static size_t UmiAnalyticsAreaChartArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiAnalyticsAreaChartArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsAreaChart *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->stacked);
    UmiArchiveWriteDouble(writer, value->opacity);
}
static void UmiAnalyticsAreaChartArchiveRead(UmiArchiveReader *reader, UmiAnalyticsAreaChart *value)
{
    value->stacked = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->opacity = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAnalyticsAreaChartArchiveValidate(const UmiAnalyticsAreaChart *value)
{
    return umi_analytics_area_chart_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_area_chart_archive_encode, umi_analytics_area_chart_archive_decode,
    UmiAnalyticsAreaChart, UmiAnalyticsAreaChartArchiveSchema, UmiAnalyticsAreaChartArchiveBound, UmiAnalyticsAreaChartArchiveWrite, UmiAnalyticsAreaChartArchiveRead, UmiAnalyticsAreaChartArchiveValidate)
