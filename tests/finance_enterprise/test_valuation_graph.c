/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_enterprise/test_valuation_graph.c
 *
 * PURPOSE:
 *   Implement the test valuation graph behavior for
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

#include "umicom/finance/enterprise/valuation_graph.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiEnterpriseValuationGraph g; UmiEnterpriseValuationTask a,b; UmiEnterpriseValuationDependency d; umi_enterprise_valuation_graph_init(&g); CHECK(umi_enterprise_valuation_task_init(&a,"a","j","a",1.0)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_task_init(&b,"b","j","b",1.0)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_graph_add_task(&g,&a)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_graph_add_task(&g,&b)==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_dependency_init(&d,"a","b")==UMI_STATUS_OK); CHECK(umi_enterprise_valuation_graph_add_dependency(&g,&d)==UMI_STATUS_OK); return 0; }
