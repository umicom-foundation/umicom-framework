/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_commission_schedule.c
 *
 * PURPOSE:
 *   Exercise define per-lot and minimum brokerage commission in integer minor units.
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
#include "umicom/trading/core/commission_schedule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/commission_schedule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingCommissionScheduleTransferEqual(const UmiTradingCommissionSchedule *a, const UmiTradingCommissionSchedule *b)
{
    return a->per_lot_minor == b->per_lot_minor &&
        a->minimum_minor == b->minimum_minor &&
        a->maximum_minor == b->maximum_minor;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingCommissionScheduleTransferTails(UmiTradingCommissionSchedule *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingCommissionScheduleTransferMalformed(const UmiTradingCommissionSchedule *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingCommissionScheduleTransferCases, UmiTradingCommissionSchedule,
    umi_trading_commission_schedule_archive_encode, umi_trading_commission_schedule_archive_decode,
    UmiTradingCommissionScheduleTransferEqual, UmiTradingCommissionScheduleTransferTails, UmiTradingCommissionScheduleTransferMalformed)

int main(void) {
    UmiTradingCommissionSchedule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_commission_schedule_init(&v,2,10,1000)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_commission_schedule_valid(&v)) return 2;
    if (UmiTradingCommissionScheduleTransferCases(&v) != 0) return 1;

     return 0;
}
