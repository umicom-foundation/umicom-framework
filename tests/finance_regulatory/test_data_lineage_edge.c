/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_data_lineage_edge.c
 *
 * PURPOSE:
 *   Implement the test data lineage edge behavior for
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

#include "umicom/finance/regulatory/data_lineage_edge.h"

int main(void)
{
    UmiDataLineageEdge record;
    CHECK(umi_reg_data_lineage_edge_init(&record, "from_node-1", "to_node-1", "Transform", 1) == UMI_STATUS_OK);
    CHECK(record.from_node[0] != '\0');
    CHECK(record.to_node[0] != '\0');
    CHECK(record.transform[0] != '\0');
    return 0;
}
