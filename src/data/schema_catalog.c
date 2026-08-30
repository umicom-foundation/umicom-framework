/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/schema_catalog.c
 *
 * PURPOSE:
 *   Implement the schema catalog behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/data/schema_catalog.h"
#include <string.h>
static size_t find_table(const UmiDatabaseSchemaCatalog *c,const char *s,const char *t){size_t i;if(c==NULL||s==NULL||t==NULL)return SIZE_MAX;for(i=0U;i<c->table_count;++i)if(strcmp(c->tables[i].schema_name,s)==0&&strcmp(c->tables[i].name,t)==0)return i;return SIZE_MAX;}
UmiStatus umi_database_schema_catalog_init(UmiDatabaseSchemaCatalog *c){if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));c->revision=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_schema_catalog_upsert(UmiDatabaseSchemaCatalog *c,const UmiDatabaseTableInfo *t){size_t i;if(c==NULL||t==NULL||t->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_table(c,t->schema_name,t->name);if(i==SIZE_MAX){if(c->table_count>=UMI_DATABASE_MAX_TABLES)return UMI_STATUS_CAPACITY_EXCEEDED;i=c->table_count++;}c->tables[i]=*t;c->revision+=1U;return UMI_STATUS_OK;}
UmiStatus umi_database_schema_catalog_find(const UmiDatabaseSchemaCatalog *c,const char *s,const char *t,UmiDatabaseTableInfo *out){size_t i;if(c==NULL||s==NULL||t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_table(c,s,t);if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=c->tables[i];return UMI_STATUS_OK;}
UmiStatus umi_database_table_add_column(UmiDatabaseTableInfo *t,const UmiDatabaseColumnInfo *c){size_t i;if(t==NULL||c==NULL||c->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;for(i=0U;i<t->column_count;++i)if(strcmp(t->columns[i].name,c->name)==0){t->columns[i]=*c;return UMI_STATUS_OK;}if(t->column_count>=UMI_DATABASE_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;t->columns[t->column_count++]=*c;return UMI_STATUS_OK;}
