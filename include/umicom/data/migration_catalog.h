/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_DATA_MIGRATION_CATALOG_H
#define UMICOM_DATA_MIGRATION_CATALOG_H
#include "umicom/data/explorer_types.h"
typedef enum UmiDatabaseMigrationState { UMI_DATABASE_MIGRATION_PENDING=1, UMI_DATABASE_MIGRATION_APPLIED=2, UMI_DATABASE_MIGRATION_FAILED=3 } UmiDatabaseMigrationState;
typedef struct UmiDatabaseMigrationInfo { uint32_t version; char name[UMI_DATABASE_NAME_CAPACITY]; char checksum[128]; UmiDatabaseMigrationState state; uint64_t applied_at; } UmiDatabaseMigrationInfo;
typedef struct UmiDatabaseMigrationCatalog { UmiDatabaseMigrationInfo items[UMI_DATABASE_MAX_MIGRATIONS]; size_t count; uint32_t current_version; uint64_t revision; } UmiDatabaseMigrationCatalog;
UmiStatus umi_database_migration_catalog_init(UmiDatabaseMigrationCatalog *catalog);
UmiStatus umi_database_migration_catalog_upsert(UmiDatabaseMigrationCatalog *catalog,const UmiDatabaseMigrationInfo *migration);
UmiStatus umi_database_migration_catalog_mark_applied(UmiDatabaseMigrationCatalog *catalog,uint32_t version,uint64_t applied_at);
size_t umi_database_migration_catalog_pending(const UmiDatabaseMigrationCatalog *catalog);
#endif
