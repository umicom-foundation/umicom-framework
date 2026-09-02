/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_signing_policy.c
 *
 * PURPOSE:
 *   Implement the test signing policy behavior for
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

#include "umicom/finance/digital_asset/signing_policy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalSigningPolicy value;
    CHECK(umi_digital_asset_signing_policy_init(&value, "POL-1", 2U, 3U, true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_signing_policy_valid(&value));
    return 0;
}
