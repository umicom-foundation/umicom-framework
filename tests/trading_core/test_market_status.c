/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_market_status.c
 *
 * PURPOSE:
 *   Exercise capture exchange phase, sequence and operational availability.
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
#include "umicom/trading/core/market_status.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/market_status.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingMarketStatusTransferEqual(const UmiTradingMarketStatus *a, const UmiTradingMarketStatus *b)
{
    return a->phase == b->phase &&
        a->sequence == b->sequence &&
        a->operational == b->operational;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingMarketStatusTransferTails(UmiTradingMarketStatus *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingMarketStatusTransferMalformed(const UmiTradingMarketStatus *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingMarketStatusTransferCases, UmiTradingMarketStatus,
    umi_trading_market_status_archive_encode, umi_trading_market_status_archive_decode,
    UmiTradingMarketStatusTransferEqual, UmiTradingMarketStatusTransferTails, UmiTradingMarketStatusTransferMalformed)

int main(void) {
    UmiTradingMarketStatus v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_market_status_init(&v,UMI_TRADING_CORE_PHASE_CONTINUOUS,1U,true)!=UMI_STATUS_OK) return 1;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(!umi_trading_market_status_valid(&v)) return 2;
    if (UmiTradingMarketStatusTransferCases(&v) != 0) return 1;

     return 0;
}
