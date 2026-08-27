/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_sql_document.c
 *
 * PURPOSE:
 *   Verify revision-safe SQL text and selected-statement extraction.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/data/workbench/sql_document.h"

int main(void)
{
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
