/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_portfolio_risk_result.c
 *
 * PURPOSE:
 *   Implement the test portfolio risk result behavior for
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

#include "umicom/finance/enterprise/portfolio_risk_result.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterprisePortfolioRiskResult r; CHECK(umi_enterprise_portfolio_risk_result_init(&r,"p",10.0,12.0,20.0)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_risk_result_init(&r,"p",12.0,10.0,20.0)==UMI_STATUS_INVALID_ARGUMENT); return 0; }
