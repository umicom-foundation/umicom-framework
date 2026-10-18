/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_parameter.h
 *
 * PURPOSE:
 *   Represent a typed bound query parameter without embedding values in generated SQL.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PARAMETER_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PARAMETER_H

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
 * Represent the data query parameter data shared with callers of this public contract.
 */
typedef struct UmiDataQueryParameter {
    char parameter_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char name[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    UmiDataValueKind kind;
    char value[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool sensitive;
} UmiDataQueryParameter;

/* Initialise a validated query parameter descriptor. */
/** Construct a complete value before replacing the destination. Any failure,
 * including allocation failure, leaves its previous bytes unchanged. Text inputs
 * may refer to its existing inline fields. This call allocates temporary storage;
 * it does not acquire external resources or synchronize concurrent callers. */
UmiStatus umi_data_query_parameter_init(UmiDataQueryParameter *item, const char *parameter_id, const char *name, UmiDataValueKind kind, const char *value, bool sensitive);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_parameter_validate(const UmiDataQueryParameter *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_query_parameter_archive_encode(const UmiDataQueryParameter *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_query_parameter_archive_decode(const void *bytes, size_t byte_count,
    UmiDataQueryParameter *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDataQueryParameter object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_data_query_parameter_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDataQueryParameter *workspace, UmiDataQueryParameter *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_data_query_parameter_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDataQueryParameter *expected, UmiDataQueryParameter *value);


#ifdef __cplusplus
}
#endif
#endif
