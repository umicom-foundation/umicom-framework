/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_snapshot.c
 *
 * PURPOSE:
 *   Implement the test market data snapshot behavior for
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

#include "umicom/finance/enterprise/market_data_snapshot.h"

int main(void)
{
    UmiEnterpriseMarketDataSnapshot s; UmiEnterpriseMarketDataUpdate u;
    CHECK(umi_enterprise_market_data_snapshot_init(&s, "snap", 100) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_update_init(&u, "X", 1.0, 1U, 99) == UMI_STATUS_OK);
    CHECK(umi_enterprise_market_data_snapshot_add(&s, &u) == UMI_STATUS_OK);
    CHECK(s.count == 1U);
    return 0;
}
