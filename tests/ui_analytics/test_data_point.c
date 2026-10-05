/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_data_point.c
 *
 * PURPOSE:
 *   Validate data_point analytics behaviour.
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
#include "umicom/ui/analytics/data_point.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/data_point.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsDataPointTransferEqual(const UmiAnalyticsDataPoint *a, const UmiAnalyticsDataPoint *b)
{
    return a->x == b->x &&
        a->y == b->y &&
        a->valid == b->valid;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsDataPointTransferTails(UmiAnalyticsDataPoint *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsDataPointTransferMalformed(const UmiAnalyticsDataPoint *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsDataPointTransferCases, UmiAnalyticsDataPoint,
    umi_analytics_data_point_archive_encode, umi_analytics_data_point_archive_decode,
    UmiAnalyticsDataPointTransferEqual, UmiAnalyticsDataPointTransferTails, UmiAnalyticsDataPointTransferMalformed)

int main(void){UmiAnalyticsDataPoint p; /* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_data_point_init(&p,1.0,2.0)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsDataPointTransferCases(&p) != 0) return 1;
 return p.y==2.0?0:2;}
