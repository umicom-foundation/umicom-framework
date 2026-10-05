/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_analytics/test_crosshair.c
 *
 * PURPOSE:
 *   Validate crosshair analytics behaviour.
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
#include "umicom/ui/analytics/crosshair.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/ui/analytics/crosshair.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiAnalyticsCrosshairTransferEqual(const UmiAnalyticsCrosshair *a, const UmiAnalyticsCrosshair *b)
{
    return a->x == b->x &&
        a->y == b->y &&
        a->visible == b->visible &&
        a->locked == b->locked;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiAnalyticsCrosshairTransferTails(UmiAnalyticsCrosshair *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiAnalyticsCrosshairTransferMalformed(const UmiAnalyticsCrosshair *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiAnalyticsCrosshairTransferCases, UmiAnalyticsCrosshair,
    umi_analytics_crosshair_archive_encode, umi_analytics_crosshair_archive_decode,
    UmiAnalyticsCrosshairTransferEqual, UmiAnalyticsCrosshairTransferTails, UmiAnalyticsCrosshairTransferMalformed)

int main(void){UmiAnalyticsCrosshair item;/* Preserve the original failure result so the caller can respond to the correct cause. */ if(umi_analytics_crosshair_init(&item)!=UMI_STATUS_OK)return 1;
    if (UmiAnalyticsCrosshairTransferCases(&item) != 0) return 1;
return (umi_analytics_crosshair_valid(&item))?0:2;}
