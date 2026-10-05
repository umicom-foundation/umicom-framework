/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/scatter_chart.c
 *
 * PURPOSE:
 *   Configure scatter point radius and optional trend presentation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/scatter_chart.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics scatter chart from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_scatter_chart_init(UmiAnalyticsScatterChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->point_radius=3.0;return UMI_STATUS_OK;}
/*
 * Check that analytics scatter chart satisfies its contract before another service relies
 * on it.
 */
int umi_analytics_scatter_chart_valid(const UmiAnalyticsScatterChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->point_radius>0.0)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsScatterChartArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x8d6975e0bad331d7);

    return schema;
}
static size_t UmiAnalyticsScatterChartArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiAnalyticsScatterChartArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsScatterChart *value)
{
    UmiArchiveWriteDouble(writer, value->point_radius);
    UmiArchiveWriteSigned(writer, (int64_t)value->trendline);
}
static void UmiAnalyticsScatterChartArchiveRead(UmiArchiveReader *reader, UmiAnalyticsScatterChart *value)
{
    value->point_radius = UmiArchiveReadDouble(reader);
    value->trendline = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
}
static UmiStatus UmiAnalyticsScatterChartArchiveValidate(const UmiAnalyticsScatterChart *value)
{
    return umi_analytics_scatter_chart_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_scatter_chart_archive_encode, umi_analytics_scatter_chart_archive_decode,
    UmiAnalyticsScatterChart, UmiAnalyticsScatterChartArchiveSchema, UmiAnalyticsScatterChartArchiveBound, UmiAnalyticsScatterChartArchiveWrite, UmiAnalyticsScatterChartArchiveRead, UmiAnalyticsScatterChartArchiveValidate)
