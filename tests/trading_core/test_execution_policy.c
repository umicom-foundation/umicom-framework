/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_execution_policy.c
 *
 * PURPOSE:
 *   Exercise define venue-count, participation and urgency bounds for execution strategies.
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
#include "umicom/trading/core/execution_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/execution_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingExecutionPolicyTransferEqual(const UmiTradingExecutionPolicy *a, const UmiTradingExecutionPolicy *b)
{
    return a->max_venues == b->max_venues &&
        a->participation_bps == b->participation_bps &&
        a->urgency == b->urgency;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingExecutionPolicyTransferTails(UmiTradingExecutionPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingExecutionPolicyTransferMalformed(const UmiTradingExecutionPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingExecutionPolicyTransferCases, UmiTradingExecutionPolicy,
    umi_trading_execution_policy_archive_encode, umi_trading_execution_policy_archive_decode,
    UmiTradingExecutionPolicyTransferEqual, UmiTradingExecutionPolicyTransferTails, UmiTradingExecutionPolicyTransferMalformed)

int main(void) {
    UmiTradingExecutionPolicy v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_execution_policy_init(&v,4U,1000U,50U)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_execution_policy_valid(&v)) return 2;
    if (UmiTradingExecutionPolicyTransferCases(&v) != 0) return 1;

     return 0;
}
