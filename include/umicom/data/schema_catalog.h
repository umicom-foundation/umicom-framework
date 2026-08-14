/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#ifndef UMICOM_DATA_SCHEMA_CATALOG_H
#define UMICOM_DATA_SCHEMA_CATALOG_H
#include "umicom/data/explorer_types.h"
typedef struct UmiDatabaseColumnInfo { char name[UMI_DATABASE_NAME_CAPACITY]; char type_name[UMI_DATABASE_NAME_CAPACITY]; int nullable; int primary_key; int generated; } UmiDatabaseColumnInfo;
typedef struct UmiDatabaseTableInfo { char schema_name[UMI_DATABASE_NAME_CAPACITY]; char name[UMI_DATABASE_NAME_CAPACITY]; UmiDatabaseColumnInfo columns[UMI_DATABASE_MAX_COLUMNS]; size_t column_count; uint64_t estimated_rows; int view; } UmiDatabaseTableInfo;
typedef struct UmiDatabaseSchemaCatalog { UmiDatabaseTableInfo tables[UMI_DATABASE_MAX_TABLES]; size_t table_count; uint64_t revision; } UmiDatabaseSchemaCatalog;
UmiStatus umi_database_schema_catalog_init(UmiDatabaseSchemaCatalog *catalog);
UmiStatus umi_database_schema_catalog_upsert(UmiDatabaseSchemaCatalog *catalog,const UmiDatabaseTableInfo *table);
UmiStatus umi_database_schema_catalog_find(const UmiDatabaseSchemaCatalog *catalog,const char *schema_name,const char *table_name,UmiDatabaseTableInfo *out_table);
UmiStatus umi_database_table_add_column(UmiDatabaseTableInfo *table,const UmiDatabaseColumnInfo *column);
#endif
