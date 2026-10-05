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

#ifdef __cplusplus
}
#endif
#endif
