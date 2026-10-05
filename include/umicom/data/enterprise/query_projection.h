/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_projection.h
 *
 * PURPOSE:
 *   Describe one selected field/alias for portable query result shapes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_PROJECTION_H
#define UMICOM_DATA_ENTERPRISE_QUERY_PROJECTION_H

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
 * Represent the data query projection data shared with callers of this public contract.
 */
typedef struct UmiDataQueryProjection {
    char projection_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char field[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char alias[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    bool hidden;
} UmiDataQueryProjection;

/* Initialise a validated query projection descriptor. */
UmiStatus umi_data_query_projection_init(UmiDataQueryProjection *item, const char *projection_id, const char *field, const char *alias);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_projection_validate(const UmiDataQueryProjection *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_query_projection_archive_encode(const UmiDataQueryProjection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_query_projection_archive_decode(const void *bytes, size_t byte_count,
    UmiDataQueryProjection *value);

#ifdef __cplusplus
}
#endif
#endif
