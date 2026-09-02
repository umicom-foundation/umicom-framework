/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/schema_catalog.h
 *
 * PURPOSE:
 *   Publish the public schema catalog contract for reusable Umicom applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_SCHEMA_CATALOG_H
#define UMICOM_DATA_SCHEMA_CATALOG_H
#include "umicom/data/explorer_types.h"
/**
 * Represent the database column info data shared with callers of this public contract.
 */
typedef struct UmiDatabaseColumnInfo { char name[UMI_DATABASE_NAME_CAPACITY]; char type_name[UMI_DATABASE_NAME_CAPACITY]; int nullable; int primary_key; int generated; } UmiDatabaseColumnInfo;
/**
 * Represent the database table info data shared with callers of this public contract.
 */
typedef struct UmiDatabaseTableInfo { char schema_name[UMI_DATABASE_NAME_CAPACITY]; char name[UMI_DATABASE_NAME_CAPACITY]; UmiDatabaseColumnInfo columns[UMI_DATABASE_MAX_COLUMNS]; size_t column_count; uint64_t estimated_rows; int view; } UmiDatabaseTableInfo;
/**
 * Represent the database schema catalog data shared with callers of this public contract.
 */
typedef struct UmiDatabaseSchemaCatalog { UmiDatabaseTableInfo tables[UMI_DATABASE_MAX_TABLES]; size_t table_count; uint64_t revision; } UmiDatabaseSchemaCatalog;
/**
 * Initialise database schema catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_schema_catalog_init(UmiDatabaseSchemaCatalog *catalog);
/**
 * Provide the database schema catalog upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_schema_catalog_upsert(UmiDatabaseSchemaCatalog *catalog,const UmiDatabaseTableInfo *table);
/**
 * Find database schema catalog while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_database_schema_catalog_find(const UmiDatabaseSchemaCatalog *catalog,const char *schema_name,const char *table_name,UmiDatabaseTableInfo *out_table);
/**
 * Provide the database table add column operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_table_add_column(UmiDatabaseTableInfo *table,const UmiDatabaseColumnInfo *column);
#endif
