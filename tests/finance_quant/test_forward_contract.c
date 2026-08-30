/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_quant/test_forward_contract.c
 *
 * PURPOSE:
 *   Implement the test forward contract behavior for
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

#include "umicom/finance/quant/forward_contract.h"

int main(void)
{
    UmiQuantForwardContract value;
    CHECK(umi_quant_forward_contract_init(&value, 100.0, 50.0, 52.0) == UMI_STATUS_OK);
    CHECK(umi_quant_forward_contract_payoff(&value) > 199.9 && umi_quant_forward_contract_payoff(&value) < 200.1);
    return 0;
}
