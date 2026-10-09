/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_diff.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema diff enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_diff.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The previous automatic-storage fixture is retained for review. Its large
 * aggregate values can exhaust a native thread stack before the first check.
 * The replacement owns those same records on the heap, checks allocation and
 * releases them after the checks, including a failed assertion path. */
#if 0
int main(void) {
    UmiDataSchemaSnapshot a,b; UmiDataSchemaTable t; UmiDataSchemaDiff d;
    umi_data_schema_snapshot_init(&a); umi_data_schema_snapshot_init(&b); CHECK(umi_data_schema_table_init(&t,"orders","orders")==UMI_STATUS_OK); CHECK(umi_data_schema_snapshot_add(&a,&t)==UMI_STATUS_OK); CHECK(umi_data_schema_snapshot_add(&b,&t)==UMI_STATUS_OK); CHECK(umi_data_schema_diff_compute(&a,&b,&d)==UMI_STATUS_OK); CHECK(d.unchanged_tables==1U); CHECK(d.compatibility==UMI_DATA_COMPATIBLE);
    return 0;
}

#endif

#include <stdlib.h>

/* Snapshots embed all bounded tables and columns. Own the pair together so
 * either success or a failed comparison has one matching release. */
typedef struct SchemaDiffFixture {
    UmiDataSchemaSnapshot before;
    UmiDataSchemaSnapshot after;
} SchemaDiffFixture;

static int CheckSchemaDiff(SchemaDiffFixture *fixture)
{
    UmiDataSchemaTable table;
    UmiDataSchemaDiff difference;
    umi_data_schema_snapshot_init(&fixture->before);
    umi_data_schema_snapshot_init(&fixture->after);
    CHECK(umi_data_schema_table_init(&table, "orders", "orders") == UMI_STATUS_OK);
    CHECK(umi_data_schema_snapshot_add(&fixture->before, &table) == UMI_STATUS_OK);
    CHECK(umi_data_schema_snapshot_add(&fixture->after, &table) == UMI_STATUS_OK);
    CHECK(umi_data_schema_diff_compute(&fixture->before, &fixture->after, &difference) == UMI_STATUS_OK);
    CHECK(difference.unchanged_tables == 1U);
    CHECK(difference.compatibility == UMI_DATA_COMPATIBLE);
    return 0;
}

int main(void)
{
    SchemaDiffFixture *fixture = calloc(1U, sizeof *fixture);
    if (fixture == NULL) { fputs("Cannot allocate schema diff fixture\n", stderr); return 1; }
    int result = CheckSchemaDiff(fixture);
    free(fixture);
    return result;
}
