/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/finance_regulatory/test_data_lineage_node.c
 *
 * PURPOSE:
 *   Implement the test data lineage node behavior for
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

#include "umicom/finance/regulatory/data_lineage_node.h"

int main(void)
{
    UmiDataLineageNode record;
    CHECK(umi_reg_data_lineage_node_init(&record, "node_id-1", "source_id-1", "Object Name", 1) == UMI_STATUS_OK);
    CHECK(record.node_id[0] != '\0');
    CHECK(record.source_id[0] != '\0');
    CHECK(record.object_name[0] != '\0');
    return 0;
}
