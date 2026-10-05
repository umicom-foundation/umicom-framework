/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/query_join.h
 *
 * PURPOSE:
 *   Describe backend-neutral joins for cost analysis and SQL generation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_QUERY_JOIN_H
#define UMICOM_DATA_ENTERPRISE_QUERY_JOIN_H

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
 * Represent the data query join data shared with callers of this public contract.
 */
typedef struct UmiDataQueryJoin {
    char join_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char left_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char right_table[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char condition[UMI_DATA_ENTERPRISE_TEXT_CAPACITY];
    bool outer_join;
} UmiDataQueryJoin;

/* Initialise a validated query join descriptor. */
UmiStatus umi_data_query_join_init(UmiDataQueryJoin *item, const char *join_id, const char *left_table, const char *right_table, const char *condition, bool outer_join);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_query_join_validate(const UmiDataQueryJoin *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_query_join_archive_encode(const UmiDataQueryJoin *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_query_join_archive_decode(const void *bytes, size_t byte_count,
    UmiDataQueryJoin *value);

#ifdef __cplusplus
}
#endif
#endif
