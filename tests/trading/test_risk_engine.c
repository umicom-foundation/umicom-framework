/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/trading/test_risk_engine.c
 *
 * PURPOSE:
 *   Validate risk engine behaviour in the trading foundation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This focused regression test uses deterministic values so changes to the trading contract are visible immediately.
 */

/* Test assertions also construct the existing fixture. Keep them active in
 * Release so the public-library regression covers the same initialized data. */
#ifdef NDEBUG
#undef NDEBUG
#endif
#include <assert.h>
#include "test_trading_common.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
#include "../value_archive/transfer_cases.h"

#include "umicom/trading/types.h"
/* Compare the complete domain value rather than using struct padding or
 * re-decoding an expected byte stream. Add new public fields to this check. */
static int UmiRiskLimitTransferEqual(const UmiRiskLimit *a, const UmiRiskLimit *b)
{
    return a->max_order_quantity == b->max_order_quantity &&
        a->max_order_notional == b->max_order_notional &&
        a->max_position_quantity == b->max_position_quantity &&
        a->max_daily_loss == b->max_daily_loss;
}
/* Bytes after a string terminator can hold obsolete data. They must not be
 * included when a value is saved or transferred to another workspace. */
static void UmiRiskLimitTransferTails(UmiRiskLimit *value)
{
    (void)value;
}
/* Damage one fixed string at a time. Refusal must precede domain string
 * reads and must leave both the destination bytes and size output unchanged. */
static int UmiRiskLimitTransferMalformed(const UmiRiskLimit *sample)
{
    (void)sample;
    return 0;
}
UMI_TEST_VALUE_TRANSFER(UmiRiskLimitTransferCases, UmiRiskLimit,
    umi_risk_limit_archive_encode, umi_risk_limit_archive_decode,
    UmiRiskLimitTransferEqual, UmiRiskLimitTransferTails, UmiRiskLimitTransferMalformed)

int main(void){
    UmiRiskLimit l={5,1000000,10,5000};assert(umi_risk_limit_valid(&l));
    if (UmiRiskLimitTransferCases(&l) != 0) return 1;
UmiOrderRequest r=test_order_request();
    UmiRiskDecision d=umi_pretrade_risk_evaluate(&r,&l,0,0);assert(d.allowed);
    r.quantity=100;d=umi_pretrade_risk_evaluate(&r,&l,0,0);assert(!d.allowed);return 0;
}
