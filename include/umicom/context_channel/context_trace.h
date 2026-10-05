/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_trace.h
 *
 * PURPOSE:
 *   Trace context routing across applications and panels with causation evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRACE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRACE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context trace data shared with callers of this public contract.
 */
typedef struct UmiContextTrace {
    uint32_t structure_size;
    char trace_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    char correlation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char route_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTrace;
/**
 * Initialise context trace from caller-provided values so later operations receive a known
 * state.
 */
void umi_context_trace_init(UmiContextTrace *state);
/**
 * Provide the context trace set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_set_field(UmiContextTrace *state,size_t field_index,const char *value);
/**
 * Provide the context trace field operation used by this module and its client
 * applications.
 */
const char *umi_context_trace_field(const UmiContextTrace *state,size_t field_index);
/**
 * Provide the context trace record success operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_record_success(UmiContextTrace *state,uint64_t sequence);
/**
 * Provide the context trace record failure operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_trace_record_failure(UmiContextTrace *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context trace satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_trace_validate(const UmiContextTrace *state);
/**
 * Provide the context trace covers sequence operation used by this module and its client
 * applications.
 */
bool umi_context_trace_covers_sequence(const UmiContextTrace *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_trace_archive_encode(const UmiContextTrace *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_trace_archive_decode(const void *bytes, size_t byte_count,
    UmiContextTrace *value);

#ifdef __cplusplus
}
#endif
#endif
