/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_settlement_finality.c
 *
 * PURPOSE:
 *   Implement the test settlement finality behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/digital_asset/settlement_finality.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalConfirmationPolicy policy;
    UmiDigitalSettlementFinality finality;
    CHECK(umi_digital_asset_confirmation_policy_init(&policy, "BTC", 1U, 6U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_settlement_finality_evaluate(&policy, 3U, &finality) == UMI_STATUS_OK);
    CHECK(finality.provisionally_settled);
    CHECK(!finality.final_settled);
    return 0;
}
