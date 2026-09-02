/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_confirmation_policy.c
 *
 * PURPOSE:
 *   Implement the test confirmation policy behavior for
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

#include "umicom/finance/digital_asset/confirmation_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalConfirmationPolicy value;
    CHECK(umi_digital_asset_confirmation_policy_init(&value, "BTC", 1U, 6U) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_confirmation_policy_valid(&value));
    return 0;
}
