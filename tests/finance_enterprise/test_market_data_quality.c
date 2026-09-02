/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_market_data_quality.c
 *
 * PURPOSE:
 *   Implement the test market data quality behavior for
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

#include "umicom/finance/enterprise/market_data_quality.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseMarketDataQuality q; CHECK(umi_enterprise_market_data_quality_init(&q,10U)==UMI_STATUS_OK); q.observed=9U; q.invalid=1U; CHECK(umi_enterprise_market_data_quality_score(&q)>0.79 && umi_enterprise_market_data_quality_score(&q)<0.81); return 0; }
