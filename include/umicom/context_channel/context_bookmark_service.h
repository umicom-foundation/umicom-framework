/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_bookmark_service.h
 *
 * PURPOSE:
 *   Coordinate bookmark creation, removal and user ownership.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_BOOKMARK_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_BOOKMARK_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context bookmark service data shared with callers of this public contract.
 */
typedef struct UmiContextBookmarkService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char bookmark_id[UMI_CONTEXT_VALUE_CAPACITY];
    char user_id[UMI_CONTEXT_VALUE_CAPACITY];
    char context_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextBookmarkService;
/**
 * Initialise context bookmark service from caller-provided values so later operations
 * receive a known state.
 */
void umi_context_bookmark_service_init(UmiContextBookmarkService *state);
/**
 * Provide the context bookmark service set field operation used by this module and its
 * client applications.
 */
UmiStatus umi_context_bookmark_service_set_field(UmiContextBookmarkService *state,size_t field_index,const char *value);
/**
 * Provide the context bookmark service field operation used by this module and its client
 * applications.
 */
const char *umi_context_bookmark_service_field(const UmiContextBookmarkService *state,size_t field_index);
/**
 * Provide the context bookmark service record success operation used by this module and
 * its client applications.
 */
UmiStatus umi_context_bookmark_service_record_success(UmiContextBookmarkService *state,uint64_t sequence);
/**
 * Provide the context bookmark service record failure operation used by this module and
 * its client applications.
 */
UmiStatus umi_context_bookmark_service_record_failure(UmiContextBookmarkService *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context bookmark service satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_context_bookmark_service_validate(const UmiContextBookmarkService *state);
/**
 * Provide the context bookmark service covers sequence operation used by this module and
 * its client applications.
 */
bool umi_context_bookmark_service_covers_sequence(const UmiContextBookmarkService *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_bookmark_service_archive_encode(const UmiContextBookmarkService *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_bookmark_service_archive_decode(const void *bytes, size_t byte_count,
    UmiContextBookmarkService *value);

#ifdef __cplusplus
}
#endif
#endif
