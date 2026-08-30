/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_portfolio_snapshot.c
 *
 * PURPOSE:
 *   Implement the test portfolio snapshot behavior for
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

#include "umicom/finance/enterprise/portfolio_snapshot.h"

int main(void){ UmiEnterprisePortfolioSnapshot s; UmiEnterprisePortfolioPosition p; CHECK(umi_enterprise_portfolio_snapshot_init(&s,"p",1)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_position_init(&p,"x",1.0,2.0)==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_snapshot_add(&s,&p)==UMI_STATUS_OK); CHECK(s.count==1U); return 0; }
