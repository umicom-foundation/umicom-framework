/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_enterprise/test_schema_table.c
 *
 * PURPOSE:
 *   Provide focused regression coverage for the schema table enterprise data capability.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_table.h"
#include "../value_archive/construction_cases.h"
#include <stdio.h>
#include <string.h>

#define CHECK(expr) do { if (!(expr)) { fprintf(stderr, "CHECK failed: %s:%d: %s\n", __FILE__, __LINE__, #expr); return 1; } } while (0)

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted schema table value. */
static int UmiDataSchemaTableConstructionCases(void)
{
    UmiDataSchemaTable *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_schema_table_init(value, "orders", "orders") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->table_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_table_init(value, oversized, "orders") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_table_init(value, value->table_id, "orders") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->table_id, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->name) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_table_init(value, "orders", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_schema_table_init(value, "orders", value->name) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->name, "orders") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void) {
    if (UmiDataSchemaTableConstructionCases() != 0) return 1;

    UmiDataSchemaTable table; UmiDataSchemaColumn col, found;
    CHECK(umi_data_schema_table_init(&table,"orders","orders")==UMI_STATUS_OK);
    CHECK(umi_data_schema_column_init(&col,"orders.id","id",UMI_DATA_VALUE_INTEGER,0U,false)==UMI_STATUS_OK);
    CHECK(umi_data_schema_table_add_column(&table,&col)==UMI_STATUS_OK);
    CHECK(umi_data_schema_table_add_column(&table,&col)==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_data_schema_table_find_column(&table,"orders.id",&found)==UMI_STATUS_OK);
    CHECK(found.kind==UMI_DATA_VALUE_INTEGER);
    return 0;
}
