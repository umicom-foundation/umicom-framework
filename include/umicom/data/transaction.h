/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/transaction.h
 *
 * PURPOSE:
 *   Declare explicit Data Server transaction handles that prevent double completion and support automatic rollback during cleanup.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_TRANSACTION_H
#define UMICOM_DATA_TRANSACTION_H

#include "umicom/base/status.h"
#include "umicom/data/data_server.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataTransaction {
    UmiDataServer *server;
    int active;
    int committed;
} UmiDataTransaction;

UmiStatus umi_data_transaction_begin(UmiDataServer *server,
                                     UmiDataTransaction *out_transaction);
UmiStatus umi_data_transaction_commit(UmiDataTransaction *transaction);
UmiStatus umi_data_transaction_rollback(UmiDataTransaction *transaction);
void umi_data_transaction_dispose(UmiDataTransaction *transaction);

#ifdef __cplusplus
}
#endif

#endif
