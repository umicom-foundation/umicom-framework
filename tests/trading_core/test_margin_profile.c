/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_margin_profile.c
 *
 * PURPOSE:
 *   Exercise define conservative initial and maintenance margin ratios in basis points.
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
#include "umicom/trading/core/margin_profile.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/margin_profile.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingMarginProfileTransferEqual(const UmiTradingMarginProfile *a, const UmiTradingMarginProfile *b)
{
    return a->initial_margin_bps == b->initial_margin_bps &&
        a->maintenance_margin_bps == b->maintenance_margin_bps &&
        a->concentration_addon_bps == b->concentration_addon_bps;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingMarginProfileTransferTails(UmiTradingMarginProfile *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingMarginProfileTransferMalformed(const UmiTradingMarginProfile *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingMarginProfileTransferCases, UmiTradingMarginProfile,
    umi_trading_margin_profile_archive_encode, umi_trading_margin_profile_archive_decode,
    UmiTradingMarginProfileTransferEqual, UmiTradingMarginProfileTransferTails, UmiTradingMarginProfileTransferMalformed)

int main(void) {
    UmiTradingMarginProfile v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_margin_profile_init(&v,5000U,3000U,500U)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_margin_profile_valid(&v)) return 2;
    if (UmiTradingMarginProfileTransferCases(&v) != 0) return 1;

     return 0;
}
