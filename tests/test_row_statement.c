/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_row_statement.c
 *
 * PURPOSE:
 *   Verify typed row values and statement ownership on a non-SQLite backend.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>
#include <string.h>

int main(void)
{
    UmiDataRow *row = NULL;
    UmiDataStatement statement;
    UmiDataServer *server = NULL;
    const UmiDataValue *value;
    assert(umi_data_row_create(4U, &row) == UMI_STATUS_OK);
    assert(umi_data_row_set_integer(row, "id", 7) == UMI_STATUS_OK);
    assert(umi_data_row_set_text(row, "name", "Studio") == UMI_STATUS_OK);
    value = umi_data_row_get(row, "name");
    assert(value != NULL && value->type == UMI_DATA_TEXT);
    assert(strcmp(value->text_value, "Studio") == 0);
    assert(umi_data_statement_create("CREATE TABLE x(y);", &statement) == UMI_STATUS_OK);
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_data_statement_execute(&statement, server) == UMI_STATUS_UNAVAILABLE);
    umi_data_server_destroy(server);
    umi_data_statement_dispose(&statement);
    umi_data_row_destroy(row);
    return 0;
}
