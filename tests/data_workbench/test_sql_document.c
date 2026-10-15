/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_sql_document.c
 *
 * PURPOSE:
 *   Verify revision-safe SQL text and selected-statement extraction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include "../value_archive/construction_cases.h"
#include <string.h>

#include "umicom/data/workbench/sql_document.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */

/* A rejected replacement must preserve the previously accepted sql document value. */
static int UmiDataSqlDocumentConstructionCases(void)
{
    UmiDataSqlDocument *value = malloc(sizeof(*value));
    unsigned char *before = malloc(sizeof(*value));
    int result = 1;
    if (value == NULL || before == NULL) goto construction_cleanup;
    UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, "sql-1", "Query", "sqlite") == UMI_STATUS_OK);
    memcpy(before, value, sizeof(*value));
    {
        char oversized[sizeof(value->document_id) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, oversized, "Query", "sqlite") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, value->document_id, "Query", "sqlite") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->document_id, "sql-1") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->title) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, "sql-1", oversized, "sqlite") != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, "sql-1", value->title, "sqlite") == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->title, "Query") == 0);
        memcpy(before, value, sizeof(*value));
    }
    {
        char oversized[sizeof(value->dialect) + 1U];
        memset(oversized, 'x', sizeof(oversized) - 1U);
        oversized[sizeof(oversized) - 1U] = '\0';
        /* Fail after earlier arguments were accepted, without clearing other fields. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, "sql-1", "Query", oversized) != UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(memcmp(before, value, sizeof(*value)) == 0);
        /* Borrowing this field must not erase it before the constructor reads it. */
        UMI_CONSTRUCTION_REQUIRE(umi_data_sql_document_init(value, "sql-1", "Query", value->dialect) == UMI_STATUS_OK);
        UMI_CONSTRUCTION_REQUIRE(strcmp(value->dialect, "sqlite") == 0);
        memcpy(before, value, sizeof(*value));
    }
    result = 0;
construction_cleanup:
    free(before);
    free(value);
    return result;
}

int main(void)
{
    if (UmiDataSqlDocumentConstructionCases() != 0) return 1;

    UmiDataSqlDocument document;
    char statement[64];
    uint64_t revision;
    assert(umi_data_sql_document_init(
               &document, "sql-1", "Query", "sqlite") == UMI_STATUS_OK);
    revision = document.revision;
    assert(umi_data_sql_document_set_text(
               &document, "LIST\nGET key", revision) == UMI_STATUS_OK);
    assert(umi_data_sql_document_set_text(
               &document, "stale", revision) == UMI_STATUS_INVALID_STATE);
    assert(umi_data_sql_document_set_selection(&document, 5U, 7U) ==
           UMI_STATUS_OK);
    assert(umi_data_sql_document_statement(
               &document, statement, sizeof(statement)) == UMI_STATUS_OK);
    assert(strcmp(statement, "GET key") == 0);
    return 0;
}
