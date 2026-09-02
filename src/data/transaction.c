/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/transaction.c
 *
 * PURPOSE:
 *   Implement transaction begin, commit, rollback and fail-safe disposal over the authoritative Data Server.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/transaction.h"

#include <string.h>

/*
 * Provide the data transaction begin operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_transaction_begin(UmiDataServer *server,
                                     UmiDataTransaction *out_transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (server == NULL || out_transaction == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    (void)memset(out_transaction, 0, sizeof(*out_transaction));
    status = umi_data_server_begin(server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    out_transaction->server = server;
    out_transaction->active = 1;
    return UMI_STATUS_OK;
}

/*
 * Provide the data transaction commit operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_transaction_commit(UmiDataTransaction *transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || !transaction->active ||
        transaction->server == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_server_commit(transaction->server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) {
        transaction->active = 0;
        transaction->committed = 1;
    }
    return status;
}

/*
 * Provide the data transaction rollback operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_transaction_rollback(UmiDataTransaction *transaction)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL || !transaction->active ||
        transaction->server == NULL) {
        return UMI_STATUS_INVALID_STATE;
    }
    status = umi_data_server_rollback(transaction->server);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) transaction->active = 0;
    return status;
}

/*
 * Release or reset state held by data transaction so the same storage can be reused
 * safely.
 */
void umi_data_transaction_dispose(UmiDataTransaction *transaction)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction == NULL) return;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (transaction->active && transaction->server != NULL) {
        (void)umi_data_server_rollback(transaction->server);
    }
    (void)memset(transaction, 0, sizeof(*transaction));
}
