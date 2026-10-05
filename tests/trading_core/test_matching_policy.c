/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_matching_policy.c
 *
 * PURPOSE:
 *   Exercise define common exchange matching priorities and self-trade prevention behaviour.
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
#include "umicom/trading/core/matching_policy.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/matching_policy.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingMatchingPolicyTransferEqual(const UmiTradingMatchingPolicy *a, const UmiTradingMatchingPolicy *b)
{
    return a->price_time_priority == b->price_time_priority &&
        a->prevent_self_trade == b->prevent_self_trade &&
        a->max_matches_per_cycle == b->max_matches_per_cycle;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingMatchingPolicyTransferTails(UmiTradingMatchingPolicy *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingMatchingPolicyTransferMalformed(const UmiTradingMatchingPolicy *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingMatchingPolicyTransferCases, UmiTradingMatchingPolicy,
    umi_trading_matching_policy_archive_encode, umi_trading_matching_policy_archive_decode,
    UmiTradingMatchingPolicyTransferEqual, UmiTradingMatchingPolicyTransferTails, UmiTradingMatchingPolicyTransferMalformed)

int main(void) {
    UmiTradingMatchingPolicy v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_matching_policy_init(&v,true,true,32U)!=UMI_STATUS_OK) return 1;
     /* Use the stable identifier comparison to choose the matching record or policy. */
     if(!umi_trading_matching_policy_valid(&v)) return 2;
    if (UmiTradingMatchingPolicyTransferCases(&v) != 0) return 1;

     return 0;
}
