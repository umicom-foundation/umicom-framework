/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/field_mapping.h
 *
 * PURPOSE:
 *   Map one logical entity field to portable column metadata and conversion semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_FIELD_MAPPING_H
#define UMICOM_DATA_ENTERPRISE_FIELD_MAPPING_H

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
 * Represent the data field mapping data shared with callers of this public contract.
 */
typedef struct UmiDataFieldMapping {
    char mapping_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char entity_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char column_name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataValueKind kind;
    bool nullable;
} UmiDataFieldMapping;

/* Initialise a validated field mapping descriptor. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_field_mapping_init(UmiDataFieldMapping *item, const char *mapping_id, const char *entity_id, const char *field_name, const char *column_name, UmiDataValueKind kind);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_field_mapping_validate(const UmiDataFieldMapping *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_field_mapping_archive_encode(const UmiDataFieldMapping *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_field_mapping_archive_decode(const void *bytes, size_t byte_count,
    UmiDataFieldMapping *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDataFieldMapping object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_data_field_mapping_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDataFieldMapping *workspace, UmiDataFieldMapping *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_field_mapping_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataFieldMapping *expected, UmiDataFieldMapping *value);


#ifdef __cplusplus
}
#endif
#endif
