/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/transaction.c
 *
 * PURPOSE:
 *   Implement transaction begin, commit, rollback and fail-safe disposal over the authoritative Data Server.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/transaction.h"

#include <string.h>

UmiStatus umi_data_transaction_begin(UmiDataServer *server,
                                     UmiDataTransaction *out_transaction)
{
    UmiStatus status;
    if (server == NULL || out_transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_transaction, 0, sizeof(*out_transaction));
    status = umi_data_server_begin(server);
    if (status != UMI_STATUS_OK) return status;
    out_transaction->server = server;
    out_transaction->active = 1;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_transaction_commit(UmiDataTransaction *transaction)
{
    UmiStatus status;
    if (transaction == NULL || !transaction->active ||
        transaction->server == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_server_commit(transaction->server);
    if (status == UMI_STATUS_OK) {
        transaction->active = 0;
        transaction->committed = 1;
    }
    return status;
}

UmiStatus umi_data_transaction_rollback(UmiDataTransaction *transaction)
{
    UmiStatus status;
    if (transaction == NULL || !transaction->active ||
        transaction->server == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_server_rollback(transaction->server);
    if (status == UMI_STATUS_OK) transaction->active = 0;
    return status;
}

void umi_data_transaction_dispose(UmiDataTransaction *transaction)
{
    if (transaction == NULL) return;
    if (transaction->active && transaction->server != NULL) {
        (void)umi_data_server_rollback(transaction->server);
    }
    (void)memset(transaction, 0, sizeof(*transaction));
}
