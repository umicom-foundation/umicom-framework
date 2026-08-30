/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/explorer_transaction.h
 *
 * PURPOSE:
 *   Publish the public explorer transaction contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_EXPLORER_TRANSACTION_H
#define UMICOM_DATA_EXPLORER_TRANSACTION_H
#include <stdint.h>
#include "umicom/data/data_server.h"
typedef struct UmiDatabaseExplorerTransaction { UmiDataServer *server; uint64_t revision; int active; } UmiDatabaseExplorerTransaction;
UmiStatus umi_database_explorer_transaction_init(UmiDatabaseExplorerTransaction *transaction,UmiDataServer *server);
UmiStatus umi_database_explorer_transaction_begin(UmiDatabaseExplorerTransaction *transaction);
UmiStatus umi_database_explorer_transaction_commit(UmiDatabaseExplorerTransaction *transaction);
UmiStatus umi_database_explorer_transaction_rollback(UmiDatabaseExplorerTransaction *transaction);
#endif
