/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_data_transactions.c
 *
 * PURPOSE:
 *   Verify memory Data Server commit and rollback semantics.
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
    UmiDataServer *server = NULL;
    UmiDataTransaction transaction;
    char value[64];
    assert(umi_data_server_create_memory(&server) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "key", "before") == UMI_STATUS_OK);
    assert(umi_data_transaction_begin(server, &transaction) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "key", "after") == UMI_STATUS_OK);
    assert(umi_data_transaction_rollback(&transaction) == UMI_STATUS_OK);
    assert(umi_data_server_get(server, "key", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "before") == 0);
    assert(umi_data_transaction_begin(server, &transaction) == UMI_STATUS_OK);
    assert(umi_data_server_set(server, "key", "committed") == UMI_STATUS_OK);
    assert(umi_data_transaction_commit(&transaction) == UMI_STATUS_OK);
    umi_data_transaction_dispose(&transaction);
    assert(umi_data_server_get(server, "key", value, sizeof(value)) == UMI_STATUS_OK);
    assert(strcmp(value, "committed") == 0);
    umi_data_server_destroy(server);
    return 0;
}
