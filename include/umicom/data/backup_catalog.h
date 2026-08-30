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
typedef struct UmiDatabaseBackupRecord { char id[UMI_DATABASE_ID_CAPACITY]; char path[UMI_DATABASE_PATH_CAPACITY]; char provider[UMI_DATABASE_NAME_CAPACITY]; char checksum[128]; uint64_t created_at; uint64_t size_bytes; int verified; } UmiDatabaseBackupRecord;
typedef struct UmiDatabaseBackupCatalog { UmiDatabaseBackupRecord items[UMI_DATABASE_MAX_BACKUPS]; size_t count; uint64_t revision; } UmiDatabaseBackupCatalog;
UmiStatus umi_database_backup_catalog_init(UmiDatabaseBackupCatalog *catalog);
UmiStatus umi_database_backup_catalog_add(UmiDatabaseBackupCatalog *catalog,const UmiDatabaseBackupRecord *record);
UmiStatus umi_database_backup_catalog_mark_verified(UmiDatabaseBackupCatalog *catalog,const char *id);
#endif
