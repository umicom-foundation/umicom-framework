/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_lineage_graph.c
 *
 * PURPOSE:
 *   Implement the test lineage graph behavior for
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
#include <string.h>
#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "check failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return __LINE__; } } while (0)

#include "umicom/finance/regulatory/lineage_graph.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void){ UmiLineageGraph g; umi_reg_lineage_graph_init(&g); CHECK(umi_reg_lineage_graph_add_node(&g,"source")==UMI_STATUS_OK); CHECK(umi_reg_lineage_graph_add_node(&g,"report")==UMI_STATUS_OK); CHECK(umi_reg_lineage_graph_add_edge(&g,"source","report")==UMI_STATUS_OK); CHECK(umi_reg_lineage_graph_add_edge(&g,"report","source")==UMI_STATUS_INVALID_STATE); return 0; }
