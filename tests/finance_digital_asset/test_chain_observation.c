/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_digital_asset/test_chain_observation.c
 *
 * PURPOSE:
 *   Implement the test chain observation behavior for
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

#include "umicom/finance/digital_asset/chain_observation.h"

int main(void)
{
    UmiDigitalChainObservation value;
    CHECK(umi_digital_asset_chain_observation_init(&value, "BTC", 900000U, "000000abc", 1000) == UMI_STATUS_OK);
    CHECK(umi_digital_asset_chain_observation_valid(&value));
    return 0;
}
