/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/schema_column.h
 *
 * PURPOSE:
 *   Describe portable column metadata used by schema diffing, ORM mapping and migrations.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_SCHEMA_COLUMN_H
#define UMICOM_DATA_ENTERPRISE_SCHEMA_COLUMN_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataSchemaColumn {
    char column_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataValueKind kind;
    uint32_t ordinal;
    bool nullable;
    bool generated;
} UmiDataSchemaColumn;

/* Initialise a validated schema column descriptor. */
UmiStatus umi_data_schema_column_init(UmiDataSchemaColumn *item, const char *column_id, const char *name, UmiDataValueKind kind, uint32_t ordinal, bool nullable);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_schema_column_validate(const UmiDataSchemaColumn *item);

#ifdef __cplusplus
}
#endif
#endif
