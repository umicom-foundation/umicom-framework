/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_schema_compare.c
 *
 * PURPOSE:
 *   Verify per-table schema changes enrich the canonical aggregate diff.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <stdlib.h>

#include "umicom/data/workbench/schema_compare.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
/* The previous automatic-storage fixture is retained for review. Its large
 * aggregate values can exhaust a native thread stack before the first check.
 * The replacement owns those same records on the heap, checks allocation and
 * releases them after the checks, including a failed assertion path. */
#if 0
int main(void)
{
    UmiDataSchemaSnapshot before;
    UmiDataSchemaSnapshot after;
    UmiDataSchemaTable table;
    UmiDataSchemaCompareModel *model =
        (UmiDataSchemaCompareModel *)calloc(1U, sizeof(*model));
    assert(model != NULL);
    umi_data_schema_snapshot_init(&before);
    umi_data_schema_snapshot_init(&after);
    assert(umi_data_schema_table_init(&table, "customers", "customers") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&before, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&after, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_table_init(&table, "orders", "orders") ==
           UMI_STATUS_OK);
    assert(umi_data_schema_snapshot_add(&after, &table) == UMI_STATUS_OK);
    assert(umi_data_schema_compare_model_build(model, &before, &after) ==
           UMI_STATUS_OK);
    assert(model->summary.added_tables == 1U);
    assert(model->change_count == 2U);
    assert(umi_data_schema_compare_model_select(model, 1U) == UMI_STATUS_OK);
    free(model);
    return 0;
}

#endif

#include <stdio.h>

/* Checks remain active in Release builds. A helper returns to the owner before
 * memory is released, so every failed check follows the same cleanup path. */
#define REQUIRE_SCHEMA(condition) do { if (!(condition)) { \
    fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; \
} } while (0)

typedef struct SchemaCompareFixture {
    UmiDataSchemaSnapshot before;
    UmiDataSchemaSnapshot after;
    UmiDataSchemaCompareModel model;
} SchemaCompareFixture;

static int CheckSchemaCompare(SchemaCompareFixture *fixture)
{
    UmiDataSchemaTable table;
    umi_data_schema_snapshot_init(&fixture->before);
    umi_data_schema_snapshot_init(&fixture->after);
    REQUIRE_SCHEMA(umi_data_schema_table_init(&table, "customers", "customers") == UMI_STATUS_OK);
    REQUIRE_SCHEMA(umi_data_schema_snapshot_add(&fixture->before, &table) == UMI_STATUS_OK);
    REQUIRE_SCHEMA(umi_data_schema_snapshot_add(&fixture->after, &table) == UMI_STATUS_OK);
    REQUIRE_SCHEMA(umi_data_schema_table_init(&table, "orders", "orders") == UMI_STATUS_OK);
    REQUIRE_SCHEMA(umi_data_schema_snapshot_add(&fixture->after, &table) == UMI_STATUS_OK);
    REQUIRE_SCHEMA(umi_data_schema_compare_model_build(&fixture->model,
        &fixture->before, &fixture->after) == UMI_STATUS_OK);
    REQUIRE_SCHEMA(fixture->model.summary.added_tables == 1U);
    REQUIRE_SCHEMA(fixture->model.change_count == 2U);
    REQUIRE_SCHEMA(umi_data_schema_compare_model_select(&fixture->model, 1U) == UMI_STATUS_OK);
    return 0;
}

int main(void)
{
    SchemaCompareFixture *fixture = calloc(1U, sizeof *fixture);
    if (fixture == NULL) { fputs("Cannot allocate schema comparison fixture\n", stderr); return 1; }
    int result = CheckSchemaCompare(fixture);
    free(fixture);
    return result;
}
