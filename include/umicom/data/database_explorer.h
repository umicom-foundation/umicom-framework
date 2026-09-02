/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/database_explorer.h
 *
 * PURPOSE:
 *   Publish the public database explorer contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_DATABASE_EXPLORER_H
#define UMICOM_DATA_DATABASE_EXPLORER_H
#include "umicom/data/backup_catalog.h"
#include "umicom/data/connection_profile.h"
#include "umicom/data/explorer_transaction.h"
#include "umicom/data/migration_catalog.h"
#include "umicom/data/query_session.h"
#include "umicom/data/schema_catalog.h"
#include "umicom/data/transfer_plan.h"
/**
 * Represent the database explorer data shared with callers of this public contract.
 */
typedef struct UmiDatabaseExplorer { UmiDataServer *server; UmiDatabaseConnectionRegistry connections; UmiDatabaseSchemaCatalog schema; UmiDatabaseQuerySession queries; UmiDatabaseExplorerTransaction transaction; UmiDatabaseMigrationCatalog migrations; UmiDatabaseTransferPlan transfers[UMI_DATABASE_MAX_TRANSFERS]; size_t transfer_count; UmiDatabaseBackupCatalog backups; uint64_t revision; } UmiDatabaseExplorer;
/**
 * Represent the database explorer snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDatabaseExplorerSnapshot { char backend[32]; char path[UMI_DATABASE_PATH_CAPACITY]; size_t records; size_t connections; size_t tables; size_t query_history; size_t pending_migrations; size_t transfers; size_t backups; int transaction_active; uint64_t revision; } UmiDatabaseExplorerSnapshot;
/**
 * Initialise database explorer from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_database_explorer_init(UmiDatabaseExplorer *explorer,UmiDataServer *server);
/**
 * Provide the database explorer snapshot operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_explorer_snapshot(const UmiDatabaseExplorer *explorer,UmiDatabaseExplorerSnapshot *out_snapshot);
/**
 * Provide the database explorer add transfer operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_explorer_add_transfer(UmiDatabaseExplorer *explorer,const UmiDatabaseTransferPlan *plan);
#endif
