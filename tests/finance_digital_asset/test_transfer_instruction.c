/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_transfer_instruction.c
 *
 * PURPOSE:
 *   Implement the test transfer instruction behavior for
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

#include "umicom/finance/digital_asset/transfer_instruction.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDigitalTransferInstruction value;
    CHECK(umi_digital_asset_transfer_instruction_init(&value, "XFER-1", "CUST-1", "bc1qdest", 500, 8, "BTC") == UMI_STATUS_OK);
    CHECK(umi_digital_asset_transfer_instruction_valid(&value));
    return 0;
}
