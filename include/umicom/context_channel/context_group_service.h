/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/context_channel/context_group_service.h
 *
 * PURPOSE:
 *   Coordinate typed colour-group creation and membership changes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CONTEXT_CHANNEL_CONTEXT_GROUP_SERVICE_H
#define UMICOM_CONTEXT_CHANNEL_CONTEXT_GROUP_SERVICE_H
#include "umicom/context_channel/context_channel.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the context group service data shared with callers of this public contract.
 */
typedef struct UmiContextGroupService {
    uint32_t structure_size;
    char operation_id[UMI_CONTEXT_VALUE_CAPACITY];
    char group_id[UMI_CONTEXT_VALUE_CAPACITY];
    char member_id[UMI_CONTEXT_VALUE_CAPACITY];
    char channel_id[UMI_CONTEXT_VALUE_CAPACITY];
    uint64_t first_sequence;
    uint64_t last_sequence;
    uint64_t item_count;
    uint64_t failure_count;
    UmiStatus status;
    bool enabled;
    uint64_t revision;
} UmiContextGroupService;
/**
 * Initialise context group service from caller-provided values so later operations receive
 * a known state.
 */
void umi_context_group_service_init(UmiContextGroupService *state);
/**
 * Provide the context group service set field operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_group_service_set_field(UmiContextGroupService *state,size_t field_index,const char *value);
/**
 * Provide the context group service field operation used by this module and its client
 * applications.
 */
const char *umi_context_group_service_field(const UmiContextGroupService *state,size_t field_index);
/**
 * Provide the context group service record success operation used by this module and its
 * client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_group_service_record_success(UmiContextGroupService *state,uint64_t sequence);
/**
 * Provide the context group service record failure operation used by this module and its
 * client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: item_count, failure_count, revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_context_group_service_record_failure(UmiContextGroupService *state,UmiStatus status,uint64_t sequence);
/**
 * Check that context group service satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_context_group_service_validate(const UmiContextGroupService *state);
/**
 * Provide the context group service covers sequence operation used by this module and its
 * client applications.
 */
bool umi_context_group_service_covers_sequence(const UmiContextGroupService *state,uint64_t sequence);
/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_context_group_service_archive_encode(const UmiContextGroupService *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_context_group_service_archive_decode(const void *bytes, size_t byte_count,
    UmiContextGroupService *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_context_group_service_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiContextGroupService *expected, UmiContextGroupService *value);


#ifdef __cplusplus
}
#endif
#endif
