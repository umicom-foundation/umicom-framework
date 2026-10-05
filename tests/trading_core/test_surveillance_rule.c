/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading_core/test_surveillance_rule.c
 *
 * PURPOSE:
 *   Exercise define reusable market-surveillance thresholds and alert severity.
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
#include "umicom/trading/core/surveillance_rule.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/core/surveillance_rule.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiTradingSurveillanceRuleTransferEqual(const UmiTradingSurveillanceRule *a, const UmiTradingSurveillanceRule *b)
{
    return a->threshold == b->threshold &&
        a->window_seconds == b->window_seconds &&
        a->severity == b->severity;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiTradingSurveillanceRuleTransferTails(UmiTradingSurveillanceRule *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiTradingSurveillanceRuleTransferMalformed(const UmiTradingSurveillanceRule *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiTradingSurveillanceRuleTransferCases, UmiTradingSurveillanceRule,
    umi_trading_surveillance_rule_archive_encode, umi_trading_surveillance_rule_archive_decode,
    UmiTradingSurveillanceRuleTransferEqual, UmiTradingSurveillanceRuleTransferTails, UmiTradingSurveillanceRuleTransferMalformed)

int main(void) {
    UmiTradingSurveillanceRule v;
     /* Preserve the original failure result so the caller can respond to the correct cause. */
     if(umi_trading_surveillance_rule_init(&v,3U,60U,UMI_TRADING_CORE_WARNING)!=UMI_STATUS_OK) return 1;
     /* Apply this operation only while the related capability or state is available. */
     if(!umi_trading_surveillance_rule_valid(&v)) return 2;
    if (UmiTradingSurveillanceRuleTransferCases(&v) != 0) return 1;

     return 0;
}
