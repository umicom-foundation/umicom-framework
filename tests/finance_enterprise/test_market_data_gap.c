/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_gap.c
 *
 * PURPOSE:
 *   Implement the test market data gap behavior for
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

#include "umicom/finance/enterprise/market_data_gap.h"

int main(void){ UmiEnterpriseMarketDataGap g; CHECK(umi_enterprise_market_data_gap_detect(&g,10U,13U)==UMI_STATUS_OK); CHECK(g.missing_count==3U); return 0; }
