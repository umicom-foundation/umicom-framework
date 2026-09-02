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
/* Provide the find table operation used by this module and its client applications. */
static size_t find_table(const UmiDatabaseSchemaCatalog *c,const char *s,const char *t){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||s==NULL||t==NULL)return SIZE_MAX;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<c->table_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(c->tables[i].schema_name,s)==0&&strcmp(c->tables[i].name,t)==0)return i;return SIZE_MAX;}
/*
 * Initialise database schema catalog from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_database_schema_catalog_init(UmiDatabaseSchemaCatalog *c){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL)return UMI_STATUS_INVALID_ARGUMENT;(void)memset(c,0,sizeof(*c));c->revision=1U;return UMI_STATUS_OK;}
/*
 * Provide the database schema catalog upsert operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_schema_catalog_upsert(UmiDatabaseSchemaCatalog *c,const UmiDatabaseTableInfo *t){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||t==NULL||t->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;i=find_table(c,t->schema_name,t->name);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c->table_count>=UMI_DATABASE_MAX_TABLES)return UMI_STATUS_CAPACITY_EXCEEDED;i=c->table_count++;}c->tables[i]=*t;c->revision+=1U;return UMI_STATUS_OK;}
/*
 * Find database schema catalog while leaving the underlying catalogue or model owned by
 * this module.
 */
UmiStatus umi_database_schema_catalog_find(const UmiDatabaseSchemaCatalog *c,const char *s,const char *t,UmiDatabaseTableInfo *out){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(c==NULL||s==NULL||t==NULL||out==NULL)return UMI_STATUS_INVALID_ARGUMENT;i=find_table(c,s,t);/* Protect caller-owned memory by checking that required state is available before it is used. */ if(i==SIZE_MAX)return UMI_STATUS_NOT_FOUND;*out=c->tables[i];return UMI_STATUS_OK;}
/*
 * Provide the database table add column operation used by this module and its client
 * applications.
 */
UmiStatus umi_database_table_add_column(UmiDatabaseTableInfo *t,const UmiDatabaseColumnInfo *c){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t==NULL||c==NULL||c->name[0]=='\0')return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0U;i<t->column_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(t->columns[i].name,c->name)==0){t->columns[i]=*c;return UMI_STATUS_OK;}/* Protect caller-owned memory by checking that required state is available before it is used. */ if(t->column_count>=UMI_DATABASE_MAX_COLUMNS)return UMI_STATUS_CAPACITY_EXCEEDED;t->columns[t->column_count++]=*c;return UMI_STATUS_OK;}
