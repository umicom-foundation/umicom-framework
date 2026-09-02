/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/backup_catalog.h
 *
 * PURPOSE:
 *   Publish the public backup catalog contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_BACKUP_CATALOG_H
#define UMICOM_DATA_BACKUP_CATALOG_H
#include "umicom/data/explorer_types.h"
/**
 * Represent the database backup record data shared with callers of this public contract.
 */
typedef struct UmiDatabaseBackupRecord { char id[UMI_DATABASE_ID_CAPACITY]; char path[UMI_DATABASE_PATH_CAPACITY]; char provider[UMI_DATABASE_NAME_CAPACITY]; char checksum[128]; uint64_t created_at; uint64_t size_bytes; int verified; } UmiDatabaseBackupRecord;
/**
 * Represent the database backup catalog data shared with callers of this public contract.
 */
typedef struct UmiDatabaseBackupCatalog { UmiDatabaseBackupRecord items[UMI_DATABASE_MAX_BACKUPS]; size_t count; uint64_t revision; } UmiDatabaseBackupCatalog;
/**
 * Initialise database backup catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_backup_catalog_init(UmiDatabaseBackupCatalog *catalog);
/**
 * Add database backup catalog only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_database_backup_catalog_add(UmiDatabaseBackupCatalog *catalog,const UmiDatabaseBackupRecord *record);
/**
 * Provide the database backup catalog mark verified operation used by this module and its
 * client applications.
 */
UmiStatus umi_database_backup_catalog_mark_verified(UmiDatabaseBackupCatalog *catalog,const char *id);
#endif
