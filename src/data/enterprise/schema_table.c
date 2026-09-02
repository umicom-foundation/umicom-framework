/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/enterprise/schema_table.c
 *
 * PURPOSE:
 *   Aggregate portable table metadata and enforce unique column identities and ordinals.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/enterprise/schema_table.h"
#include <string.h>

/* Table identity is established before columns are collected. */
UmiStatus umi_data_schema_table_init(UmiDataSchemaTable *table, const char *table_id, const char *name) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (table == NULL || table_id == NULL || name == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(table,0,sizeof(*table));
    status=umi_data_enterprise_copy_text(table->table_id,sizeof(table->table_id),table_id); /* Preserve the original failure result so the caller can respond to the correct cause. */ if(status!=UMI_STATUS_OK)return status;
    return umi_data_enterprise_copy_text(table->name,sizeof(table->name),name);
}
/* Duplicate metadata is rejected early so generated migrations remain deterministic. */
UmiStatus umi_data_schema_table_add_column(UmiDataSchemaTable *table, const UmiDataSchemaColumn *column) {
    size_t i; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||column==NULL)return UMI_STATUS_INVALID_ARGUMENT; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_data_schema_column_validate(column)!=UMI_STATUS_OK)return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for(i=0;i<table->column_count;++i) /* Preserve the original failure result so the caller can respond to the correct cause. */ if(strcmp(table->columns[i].column_id,column->column_id)==0||strcmp(table->columns[i].name,column->name)==0||table->columns[i].ordinal==column->ordinal)return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (table->column_count >= UMI_DATA_ENTERPRISE_MAX_COLUMNS) return UMI_STATUS_CAPACITY_EXCEEDED;
    table->columns[table->column_count++] = *column;
    return UMI_STATUS_OK;
}
/* Lookup returns a snapshot rather than exposing mutable internal storage. */
UmiStatus umi_data_schema_table_find_column(const UmiDataSchemaTable *table,const char *column_id,UmiDataSchemaColumn *out_column){size_t i;/* Protect caller-owned memory by checking that required state is available before it is used. */ if(table==NULL||column_id==NULL||out_column==NULL)return UMI_STATUS_INVALID_ARGUMENT;/* Visit each bounded item once so every record receives the same rule. */ for(i=0;i<table->column_count;++i)/* Protect caller-owned memory by checking that required state is available before it is used. */ if(strcmp(table->columns[i].column_id,column_id)==0){*out_column=table->columns[i];return UMI_STATUS_OK;}return UMI_STATUS_NOT_FOUND;}
