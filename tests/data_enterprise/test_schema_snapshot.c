/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_snapshot.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema snapshot enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_snapshot.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiDataSchemaSnapshot registry;
    UmiDataSchemaTable item;
    UmiDataSchemaTable found;
    umi_data_schema_snapshot_init(&registry);
    CHECK(umi_data_schema_table_init(&item,"orders","orders") == UMI_STATUS_OK);
    CHECK(umi_data_schema_snapshot_add(&registry, &item) == UMI_STATUS_OK);
    CHECK(umi_data_schema_snapshot_add(&registry, &item) == UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_schema_snapshot_find(&registry, item.table_id, &found) == UMI_STATUS_OK);
    CHECK(strcmp(found.table_id, item.table_id) == 0);
    CHECK(umi_data_schema_snapshot_count(&registry) == 1U);
    return 0;
}
