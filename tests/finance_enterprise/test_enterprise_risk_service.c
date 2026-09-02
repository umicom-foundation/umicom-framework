/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_enterprise_risk_service.c
 *
 * PURPOSE:
 *   Implement the test enterprise risk service behavior for
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

#include "umicom/finance/enterprise/enterprise_risk_service.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){UmiEnterpriseRiskService s;UmiEnterpriseValuationWorker w;UmiEnterpriseMarketDataUpdate u;UmiEnterpriseRiskReport r;CHECK(umi_enterprise_risk_service_init(&s,100,1)==UMI_STATUS_OK);CHECK(umi_enterprise_valuation_worker_init(&w,"w",1U)==UMI_STATUS_OK);CHECK(umi_enterprise_valuation_worker_pool_add(&s.valuation.grid.workers,&w)==UMI_STATUS_OK);CHECK(umi_enterprise_market_data_update_init(&u,"x",1.0,1U,1)==UMI_STATUS_OK);CHECK(umi_enterprise_market_data_service_publish(&s.valuation.market_data,&u)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_report_init(&r,"p",1,1.0,2.0,3.0,0.0)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_snapshot_add(&s.risk,&r)==UMI_STATUS_OK);CHECK(umi_enterprise_risk_service_ready(&s)==1);return 0;}
