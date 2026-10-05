/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/enterprise/data_operation.h
 *
 * PURPOSE:
 *   Describe one reviewable Data Server operation for queueing, audit and cancellation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_ENTERPRISE_DATA_OPERATION_H
#define UMICOM_DATA_ENTERPRISE_DATA_OPERATION_H

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
 * Represent the data operation data shared with callers of this public contract.
 */
typedef struct UmiDataOperation {
    char operation_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char session_id[UMI_DATA_ENTERPRISE_ID_CAPACITY];
    char operation_kind[64];
    uint64_t submitted_at;
    uint32_t priority;
    bool cancellable;
} UmiDataOperation;

/* Initialise a validated data operation descriptor. */
UmiStatus umi_data_data_operation_init(UmiDataOperation *item, const char *operation_id, const char *session_id, const char *operation_kind, uint64_t submitted_at, uint32_t priority);
/* Validate invariants before the descriptor is admitted to a catalogue or plan. */
UmiStatus umi_data_data_operation_validate(const UmiDataOperation *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_data_data_operation_archive_encode(const UmiDataOperation *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_data_data_operation_archive_decode(const void *bytes, size_t byte_count,
    UmiDataOperation *value);

#ifdef __cplusplus
}
#endif
#endif
