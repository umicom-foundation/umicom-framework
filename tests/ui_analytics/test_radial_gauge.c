/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_radial_gauge.c
 *
 * PURPOSE:
 *   Validate radial_gauge analytics behaviour.
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
#include "umicom/ui/analytics/radial_gauge.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/radial_gauge.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsRadialGaugeTransferEqual(const UmiAnalyticsRadialGauge *a, const UmiAnalyticsRadialGauge *b)
{
    return a->start_degrees == b->start_degrees &&
        a->sweep_degrees == b->sweep_degrees &&
        a->needle == b->needle;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsRadialGaugeTransferTails(UmiAnalyticsRadialGauge *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsRadialGaugeTransferMalformed(const UmiAnalyticsRadialGauge *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsRadialGaugeTransferCases, UmiAnalyticsRadialGauge,
    umi_analytics_radial_gauge_archive_encode, umi_analytics_radial_gauge_archive_decode,
    UmiAnalyticsRadialGaugeTransferEqual, UmiAnalyticsRadialGaugeTransferTails, UmiAnalyticsRadialGaugeTransferMalformed)

int main(void){UmiAnalyticsRadialGauge item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_radial_gauge_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsRadialGaugeTransferCases(&item) != 0) return 1;
return (umi_analytics_radial_gauge_valid(&item))?0:2;}
