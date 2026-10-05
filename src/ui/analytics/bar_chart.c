/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/analytics/bar_chart.c
 *
 * PURPOSE:
 *   Configure grouped bar-chart orientation and relative gap.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/analytics/bar_chart.h"
#include "../../base/value_archive_internal.h"

#include <string.h>
/*
 * Initialise analytics bar chart from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_bar_chart_init(UmiAnalyticsBarChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;memset(item,0,sizeof *item);item->orientation=UMI_ANALYTICS_VERTICAL;item->gap_ratio=0.2;return UMI_STATUS_OK;}
/*
 * Check that analytics bar chart satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_bar_chart_valid(const UmiAnalyticsBarChart *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return (item->orientation>=UMI_ANALYTICS_HORIZONTAL&&item->orientation<=UMI_ANALYTICS_VERTICAL&&item->gap_ratio>=0.0&&item->gap_ratio<1.0)?1:0;}

/* State transfer belongs to this Framework value owner. Explicit fields keep
 * padding and unused text out of saved data. Extend both directions and the
 * schema identity when adding a field; migrate incompatible saved state
 * deliberately rather than interpreting it as a different record. These
 * functions never activate a provider, execute a command or perform I/O. */
static uint64_t UmiAnalyticsBarChartArchiveSchema(void)
{
    uint64_t schema = UINT64_C(0x0c5123b12b9f0269);

    return schema;
}
static size_t UmiAnalyticsBarChartArchiveBound(void)
{
    return UMI_VALUE_ARCHIVE_HEADER_SIZE +
        8U +
        8U;
}
static void UmiAnalyticsBarChartArchiveWrite(UmiArchiveWriter *writer, const UmiAnalyticsBarChart *value)
{
    UmiArchiveWriteSigned(writer, (int64_t)value->orientation);
    UmiArchiveWriteDouble(writer, value->gap_ratio);
}
static void UmiAnalyticsBarChartArchiveRead(UmiArchiveReader *reader, UmiAnalyticsBarChart *value)
{
    value->orientation = (UmiAnalyticsOrientation)UmiArchiveReadSigned(reader, INT_MIN, INT_MAX);
    value->gap_ratio = UmiArchiveReadDouble(reader);
}
static UmiStatus UmiAnalyticsBarChartArchiveValidate(const UmiAnalyticsBarChart *value)
{
    return umi_analytics_bar_chart_valid(value) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
UMI_DEFINE_VALUE_ARCHIVE(umi_analytics_bar_chart_archive_encode, umi_analytics_bar_chart_archive_decode,
    UmiAnalyticsBarChart, UmiAnalyticsBarChartArchiveSchema, UmiAnalyticsBarChartArchiveBound, UmiAnalyticsBarChartArchiveWrite, UmiAnalyticsBarChartArchiveRead, UmiAnalyticsBarChartArchiveValidate)
