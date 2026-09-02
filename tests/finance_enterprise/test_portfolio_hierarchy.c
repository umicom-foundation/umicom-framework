/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_portfolio_hierarchy.c
 *
 * PURPOSE:
 *   Implement the test portfolio hierarchy behavior for
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

#include "umicom/finance/enterprise/portfolio_hierarchy.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterprisePortfolioHierarchy h; UmiEnterprisePortfolioHierarchyNode n={{0},{0},1.0}; umi_enterprise_portfolio_hierarchy_init(&h); CHECK(umi_quant_copy_text(n.node_id,sizeof n.node_id,"root")==UMI_STATUS_OK); CHECK(umi_enterprise_portfolio_hierarchy_add(&h,&n)==UMI_STATUS_OK); CHECK(h.count==1U); return 0; }
