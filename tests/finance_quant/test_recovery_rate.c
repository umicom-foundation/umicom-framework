/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_recovery_rate.c
 *
 * PURPOSE:
 *   Implement the test recovery rate behavior for
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

#include "umicom/finance/quant/recovery_rate.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiQuantRecoveryRate value;
    CHECK(umi_quant_recovery_rate_init(&value, 0.40) == UMI_STATUS_OK);
    CHECK(umi_quant_recovery_rate_loss_given_default(&value) > 0.59 && umi_quant_recovery_rate_loss_given_default(&value) < 0.61);
    return 0;
}
