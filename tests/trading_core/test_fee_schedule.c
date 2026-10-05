/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_fee_schedule.c
 *
 * PURPOSE:
 *   Exercise define maker/taker exchange fees in minor units per lot.
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
#include "umicom/trading/core/fee_schedule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/fee_schedule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingFeeScheduleTransferEqual(const UmiTradingFeeSchedule *a, const UmiTradingFeeSchedule *b)
{
    return a->maker_minor_per_lot == b->maker_minor_per_lot &&
        a->taker_minor_per_lot == b->taker_minor_per_lot &&
        a->regulatory_minor_per_lot == b->regulatory_minor_per_lot;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingFeeScheduleTransferTails(UmiTradingFeeSchedule *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingFeeScheduleTransferMalformed(const UmiTradingFeeSchedule *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingFeeScheduleTransferCases, UmiTradingFeeSchedule,
    umi_trading_fee_schedule_archive_encode, umi_trading_fee_schedule_archive_decode,
    UmiTradingFeeScheduleTransferEqual, UmiTradingFeeScheduleTransferTails, UmiTradingFeeScheduleTransferMalformed)

int main(void) {
    UmiTradingFeeSchedule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_fee_schedule_init(&v,1,2,1)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_fee_schedule_valid(&v)) return 2;
    if (UmiTradingFeeScheduleTransferCases(&v) != 0) return 1;

     return 0;
}
