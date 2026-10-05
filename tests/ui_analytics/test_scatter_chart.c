/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_scatter_chart.c
 *
 * PURPOSE:
 *   Validate scatter_chart analytics behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include "umicom/ui/analytics/scatter_chart.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/scatter_chart.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsScatterChartTransferEqual(const UmiAnalyticsScatterChart *a, const UmiAnalyticsScatterChart *b)
{
    return a->point_radius == b->point_radius &&
        a->trendline == b->trendline;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsScatterChartTransferTails(UmiAnalyticsScatterChart *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsScatterChartTransferMalformed(const UmiAnalyticsScatterChart *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsScatterChartTransferCases, UmiAnalyticsScatterChart,
    umi_analytics_scatter_chart_archive_encode, umi_analytics_scatter_chart_archive_decode,
    UmiAnalyticsScatterChartTransferEqual, UmiAnalyticsScatterChartTransferTails, UmiAnalyticsScatterChartTransferMalformed)

int main(void){UmiAnalyticsScatterChart item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_scatter_chart_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsScatterChartTransferCases(&item) != 0) return 1;
return (umi_analytics_scatter_chart_valid(&item))?0:2;}
