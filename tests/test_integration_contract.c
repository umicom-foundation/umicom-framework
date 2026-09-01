/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_integration_contract.c
 *
 * PURPOSE:
 *   Exercise one Suite and Inter-Application Runtime Foundation behaviour.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

/*
 * This file keeps one part of the public runtime small and explicit. Product
 * code uses these contracts instead of reaching into another application's
 * private state or private headers.
 */

#include "umicom/integration/contract.h"

#include <stdio.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s\n", #expr); return 1; } } while (0)

int main(void) {
    UmiIntegrationContract p={"umicom.account/1",1U,4U};
    UmiIntegrationContract c={"umicom.account/1",1U,2U};
    UmiIntegrationContract bad={"umicom.account/1",2U,0U};
    CHECK(umi_integration_contract_compatible(&p,&c));
    CHECK(!umi_integration_contract_compatible(&p,&bad));
    return 0;
}
