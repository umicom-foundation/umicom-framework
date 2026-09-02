/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_table.h
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
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_TABLE_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_TABLE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"
#include "umicom/data/enterprise/schema_column.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data schema table data shared with callers of this public contract.
 */
typedef struct UmiDataSchemaTable {
    char table_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataSchemaColumn columns[UMI_DATA_ENTERPRISE_MAX_COLUMNS];
    size_t column_count;
    bool temporary;
} UmiDataSchemaTable;
/* Initialise an empty table descriptor. */
UmiStatus umi_data_schema_table_init(UmiDataSchemaTable *table, const char *table_id, const char *name);
/* Add one column while rejecting duplicate IDs, names and ordinals. */
UmiStatus umi_data_schema_table_add_column(UmiDataSchemaTable *table, const UmiDataSchemaColumn *column);
/* Locate a table column by stable identifier. */
UmiStatus umi_data_schema_table_find_column(const UmiDataSchemaTable *table, const char *column_id, UmiDataSchemaColumn *out_column);

#ifdef __cplusplus
}
#endif
#endif
