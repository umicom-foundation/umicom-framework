/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_threshold_band.c
 *
 * PURPOSE:
 *   Validate threshold_band analytics behaviour.
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
#include "umicom/ui/analytics/threshold_band.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/threshold_band.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsThresholdBandTransferEqual(const UmiAnalyticsThresholdBand *a, const UmiAnalyticsThresholdBand *b)
{
    return a->lower == b->lower &&
        a->upper == b->upper &&
        a->severity == b->severity;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsThresholdBandTransferTails(UmiAnalyticsThresholdBand *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsThresholdBandTransferMalformed(const UmiAnalyticsThresholdBand *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsThresholdBandTransferCases, UmiAnalyticsThresholdBand,
    umi_analytics_threshold_band_archive_encode, umi_analytics_threshold_band_archive_decode,
    UmiAnalyticsThresholdBandTransferEqual, UmiAnalyticsThresholdBandTransferTails, UmiAnalyticsThresholdBandTransferMalformed)

int main(void){UmiAnalyticsThresholdBand item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_threshold_band_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsThresholdBandTransferCases(&item) != 0) return 1;
return (umi_analytics_threshold_band_valid(&item))?0:2;}
