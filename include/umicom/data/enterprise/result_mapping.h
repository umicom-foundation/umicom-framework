/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/result_mapping.h
 *
 * PURPOSE:
 *   Describe how one result column maps back into an ORM field.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_RESULT_MAPPING_H
#define UMICOM_DATA_ENTERPRISE_RESULT_MAPPING_H

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
 * Represent the data result mapping data shared with callers of this public contract.
 */
typedef struct UmiDataResultMapping {
    char mapping_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    uint32_t column_ordinal;
    UmiDataValueKind kind;
} UmiDataResultMapping;

/* Initialise a validated result mapping descriptor. */
UmiStatus umi_data_result_mapping_init(UmiDataResultMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, uint32_t column_ordinal, UmiDataValueKind kind);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_result_mapping_validate(const UmiDataResultMapping *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_result_mapping_archive_encode(const UmiDataResultMapping *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_result_mapping_archive_decode(const void *bytes, size_t byte_count,
    UmiDataResultMapping *value);

#ifdef __cplusplus
}
#endif
#endif
