/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_snapshot_store.c
 *
 * PURPOSE:
 *   Implement the test market data snapshot store behavior for
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

#include "umicom/finance/enterprise/market_data_snapshot_store.h"

int main(void){ UmiEnterpriseMarketDataSnapshotStore store; UmiEnterpriseMarketDataSnapshot s; umi_enterprise_market_data_snapshot_store_init(&store); CHECK(umi_enterprise_market_data_snapshot_init(&s,"s",1)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_snapshot_store_append(&store,&s)==UMI_STATUS_OK); CHECK(umi_enterprise_market_data_snapshot_store_latest(&store)->as_of_ms==1); return 0; }
