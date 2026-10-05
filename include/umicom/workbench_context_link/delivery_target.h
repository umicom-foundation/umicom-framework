/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/delivery_target.h
 *
 * PURPOSE:
 *   Define the reusable context delivery target contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_TARGET_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_DELIVERY_TARGET_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link delivery target data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkDeliveryTarget {
    uint32_t structure_size;
    char target_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char application_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    UmiContextKind context_kind;
    UmiContextChannelColour colour;
    UmiWorkbenchContextLinkMode mode;
    UmiWorkbenchContextLinkState state;
    UmiWorkbenchContextLinkOrigin origin;
    UmiWorkbenchContextLinkPriority priority;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextLinkDeliveryTarget;

/**
 * Initialise workbench context link delivery target from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_delivery_target_init(UmiWorkbenchContextLinkDeliveryTarget *record,
                                           const char *identity);
/**
 * Check that workbench context link delivery target satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_delivery_target_validate(
    const UmiWorkbenchContextLinkDeliveryTarget *record);
/**
 * Copy workbench context link delivery target into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_delivery_target_copy(
    UmiWorkbenchContextLinkDeliveryTarget *destination,
    const UmiWorkbenchContextLinkDeliveryTarget *source);
/**
 * Provide the workbench context link delivery target hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_delivery_target_hash(
    const UmiWorkbenchContextLinkDeliveryTarget *record);
/**
 * Provide the workbench context link delivery target set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_target_set_primary(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    const char *value);
/**
 * Provide the workbench context link delivery target set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_delivery_target_set_secondary(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    const char *value);
/**
 * Provide the workbench context link delivery target touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_delivery_target_touch(
    UmiWorkbenchContextLinkDeliveryTarget *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_delivery_target_archive_encode(const UmiWorkbenchContextLinkDeliveryTarget *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_delivery_target_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkDeliveryTarget *value);

#ifdef __cplusplus
}
#endif

#endif
