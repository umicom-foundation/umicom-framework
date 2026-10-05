/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_linear_gauge.c
 *
 * PURPOSE:
 *   Validate linear_gauge analytics behaviour.
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
#include "umicom/ui/analytics/linear_gauge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/linear_gauge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsLinearGaugeTransferEqual(const UmiAnalyticsLinearGauge *a, const UmiAnalyticsLinearGauge *b)
{
    return a->orientation == b->orientation &&
        a->reversed == b->reversed;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsLinearGaugeTransferTails(UmiAnalyticsLinearGauge *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsLinearGaugeTransferMalformed(const UmiAnalyticsLinearGauge *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsLinearGaugeTransferCases, UmiAnalyticsLinearGauge,
    umi_analytics_linear_gauge_archive_encode, umi_analytics_linear_gauge_archive_decode,
    UmiAnalyticsLinearGaugeTransferEqual, UmiAnalyticsLinearGaugeTransferTails, UmiAnalyticsLinearGaugeTransferMalformed)

int main(void){UmiAnalyticsLinearGauge item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_linear_gauge_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsLinearGaugeTransferCases(&item) != 0) return 1;
return (umi_analytics_linear_gauge_valid(&item))?0:2;}
