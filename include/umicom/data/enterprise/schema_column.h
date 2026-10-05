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
#include "umicom/base/value_archive.h"
#include <stdint.h>
#include <stdbool.h>
#include "umicom/base/status.h"
#include "umicom/data/enterprise/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data schema column data shared with callers of this public contract.
 */
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

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_schema_column_archive_encode(const UmiDataSchemaColumn *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_schema_column_archive_decode(const void *bytes, size_t byte_count,
    UmiDataSchemaColumn *value);

#ifdef __cplusplus
}
#endif
#endif
