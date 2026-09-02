/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/migration_catalog.h
 *
 * PURPOSE:
 *   Publish the public migration catalog contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_MIGRATION_CATALOG_H
#define UMICOM_DATA_MIGRATION_CATALOG_H
#include "umicom/data/explorer_types.h"
/**
 * List the named database migration state values accepted by this public contract.
 */
typedef enum UmiDatabaseMigrationState { UMI_DATABASE_MIGRATION_PENDING=1, UMI_DATABASE_MIGRATION_APPLIED=2, UMI_DATABASE_MIGRATION_FAILED=3 } UmiDatabaseMigrationState;
/**
 * Represent the database migration info data shared with callers of this public contract.
 */
typedef struct UmiDatabaseMigrationInfo { uint32_t version; char name[UMI_DATABASE_NAME_CAPACITY]; char checksum[128]; UmiDatabaseMigrationState state; uint64_t applied_at; } UmiDatabaseMigrationInfo;
/**
 * Represent the database migration catalog data shared with callers of this public
 * contract.
 */
typedef struct UmiDatabaseMigrationCatalog { UmiDatabaseMigrationInfo items[UMI_DATABASE_MAX_MIGRATIONS]; size_t count; uint32_t current_version; uint64_t revision; } UmiDatabaseMigrationCatalog;
/**
 * Initialise database migration catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_migration_catalog_init(UmiDatabaseMigrationCatalog *catalog);
/**
 * Provide the database migration catalog upsert operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_migration_catalog_upsert(UmiDatabaseMigrationCatalog *catalog,const UmiDatabaseMigrationInfo *migration);
/**
 * Provide the database migration catalog mark applied operation used by this module and
 * its client applications.
 */
UmiStatus umi_database_migration_catalog_mark_applied(UmiDatabaseMigrationCatalog *catalog,uint32_t version,uint64_t applied_at);
/**
 * Provide the database migration catalog pending operation used by this module and its
 * client applications.
 */
size_t umi_database_migration_catalog_pending(const UmiDatabaseMigrationCatalog *catalog);
#endif
