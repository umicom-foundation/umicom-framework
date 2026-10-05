/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/line_chart.c
 *
 * PURPOSE:
 *   Configure line-chart interpolation and marker semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/line_chart.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics line chart from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_line_chart_init(UmiAnalyticsLineChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->stroke_width=1.0;return UMI_STATUS_OK;}
/*
 * Check that analytics line chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_line_chart_valid(const UmiAnalyticsLineChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->stroke_width>0.0)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsLineChartArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0xd29e068e84641487);

    return schema;
}
static size_t UmiAnalyticsLineChartArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U +
        8U;
}
static void UmiAnalyticsLineChartArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsLineChart *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->smooth);
    UmiArchiveWriteSigned(writer, (int64_t)value->markers);
    UmiArchiveWriteDouble(writer, value->stroke_width);
}
static void UmiAnalyticsLineChartArchiveRead(UmiArchiveReader *reader, UmiAnalyticsLineChart *value)
{
    value->smooth = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->markers = (int)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->stroke_width = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAnalyticsLineChartArchiveValidate(const UmiAnalyticsLineChart *value)
{
    return umi_analytics_line_chart_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_line_chart_archive_encode, umi_analytics_line_chart_archive_decode,
    UmiAnalyticsLineChart, UmiAnalyticsLineChartArchiveSchema, UmiAnalyticsLineChartArchiveBound, UmiAnalyticsLineChartArchiveWrite, UmiAnalyticsLineChartArchiveRead, UmiAnalyticsLineChartArchiveValidate)
