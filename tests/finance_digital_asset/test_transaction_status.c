/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transaction_status.c
 *
 * PURPOSE:
 *   Implement the test transaction status behavior for
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

#include "umicom/finance/digital_asset/transaction_status.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    CHECK(umi_digital_asset_transaction_status_can_transition(UMI_DIGITAL_TX_CREATED, UMI_DIGITAL_TX_SIGNING));
    CHECK(umi_digital_asset_transaction_status_can_transition(UMI_DIGITAL_TX_CONFIRMED, UMI_DIGITAL_TX_FINAL));
    CHECK(!umi_digital_asset_transaction_status_can_transition(UMI_DIGITAL_TX_FINAL, UMI_DIGITAL_TX_CREATED));
    return 0;
}
