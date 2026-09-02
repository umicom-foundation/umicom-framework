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
/**
 * Represent the database explorer transaction data shared with callers of this public
 * contract.
 */
typedef struct UmiDatabaseExplorerTransaction { UmiDataServer *server; uint64_t revision; int active; } UmiDatabaseExplorerTransaction;
/**
 * Initialise database explorer transaction from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_explorer_transaction_init(UmiDatabaseExplorerTransaction *transaction,UmiDataServer *server);
/**
 * Provide the database explorer transaction begin operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_explorer_transaction_begin(UmiDatabaseExplorerTransaction *transaction);
/**
 * Provide the database explorer transaction commit operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_explorer_transaction_commit(UmiDatabaseExplorerTransaction *transaction);
/**
 * Provide the database explorer transaction rollback operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_explorer_transaction_rollback(UmiDatabaseExplorerTransaction *transaction);
#endif
