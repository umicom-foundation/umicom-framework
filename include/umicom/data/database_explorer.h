/*-----------------------------------------------------------------------------
 * Umicom Framework - authoritative Database Explorer orchestration
 * Created by: Sammy Hegab | Organisation: Umicom Foundation | Licence: MIT
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
typedef struct UmiDatabaseExplorer { UmiDataServer *server; UmiDatabaseConnectionRegistry connections; UmiDatabaseSchemaCatalog schema; UmiDatabaseQuerySession queries; UmiDatabaseExplorerTransaction transaction; UmiDatabaseMigrationCatalog migrations; UmiDatabaseTransferPlan transfers[UMI_DATABASE_MAX_TRANSFERS]; size_t transfer_count; UmiDatabaseBackupCatalog backups; uint64_t revision; } UmiDatabaseExplorer;
typedef struct UmiDatabaseExplorerSnapshot { char backend[32]; char path[UMI_DATABASE_PATH_CAPACITY]; size_t records; size_t connections; size_t tables; size_t query_history; size_t pending_migrations; size_t transfers; size_t backups; int transaction_active; uint64_t revision; } UmiDatabaseExplorerSnapshot;
UmiStatus umi_database_explorer_init(UmiDatabaseExplorer *explorer,UmiDataServer *server);
UmiStatus umi_database_explorer_snapshot(const UmiDatabaseExplorer *explorer,UmiDatabaseExplorerSnapshot *out_snapshot);
UmiStatus umi_database_explorer_add_transfer(UmiDatabaseExplorer *explorer,const UmiDatabaseTransferPlan *plan);
#endif
