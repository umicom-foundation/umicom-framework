/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_key_reference.c
 *
 * PURPOSE:
 *   Implement the test key reference behavior for
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

#include "umicom/finance/digital_asset/key_reference.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalKeyReference value;
    CHECK(umi_digital_asset_key_reference_init(&value, "KEY-1", "hsm://slot/1", true) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_key_reference_valid(&value));
    CHECK(value.hardware_backed);
    return 0;
}
