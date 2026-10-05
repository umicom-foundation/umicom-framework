/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_transport.h
 *
 * PURPOSE:
 *   Describe transport-neutral context delivery endpoints and capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSPORT_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_TRANSPORT_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context transport data shared with callers of this public contract.
 */
typedef struct UmiContextTransport {
    uint32_t structure_size;
    char transport_id[UMI_CONTEXT_VALUE_CAPACITY];
    char endpoint_id[UMI_CONTEXT_VALUE_CAPACITY];
    char protocol[UMI_CONTEXT_VALUE_CAPACITY];
    char address[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextTransport;
/**
 * Initialise context transport from caller-provided values so later operations receive a
 * known state.
 */
void umi_context_transport_init(UmiContextTransport *state);
/**
 * Provide the context transport set field operation used by this module and its client
 * applications.
 */
UmiStatus umi_context_transport_set_field(UmiContextTransport *state,size_t field_index,const char *value);
/**
 * Provide the context transport field operation used by this module and its client
 * applications.
 */
const char *umi_context_transport_field(const UmiContextTransport *state,size_t field_index);
/**
 * Provide the context transport record success operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transport_record_success(UmiContextTransport *state,uint64_t sequence);
/**
 * Provide the context transport record failure operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_transport_record_failure(UmiContextTransport *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context transport satisfies its contract before another service relies on it.
 */
UmiStatus umi_context_transport_validate(const UmiContextTransport *state);
/**
 * Provide the context transport covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_transport_covers_sequence(const UmiContextTransport *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_transport_archive_encode(const UmiContextTransport *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_transport_archive_decode(const void *bytes, size_t byte_count,
    UmiContextTransport *value);

#ifdef __cplusplus
}
#endif
#endif
