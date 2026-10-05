/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/account_bridge.h
 *
 * PURPOSE:
 *   Define the reusable account context bridge contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_ACCOUNT_BRIDGE_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_ACCOUNT_BRIDGE_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link account bridge data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkAccountBridge {
    uint32_t structure_size;
    char bridge_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkAccountBridge;

/**
 * Initialise workbench context link account bridge from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_account_bridge_init(UmiWorkbenchContextLinkAccountBridge *record,
                                           const char *identity);
/**
 * Check that workbench context link account bridge satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_account_bridge_validate(
    const UmiWorkbenchContextLinkAccountBridge *record);
/**
 * Copy workbench context link account bridge into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_account_bridge_copy(
    UmiWorkbenchContextLinkAccountBridge *destination,
    const UmiWorkbenchContextLinkAccountBridge *source);
/**
 * Provide the workbench context link account bridge hash operation used by this module and
 * its client applications.
 */
uint64_t umi_workbench_context_link_account_bridge_hash(
    const UmiWorkbenchContextLinkAccountBridge *record);
/**
 * Provide the workbench context link account bridge set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_account_bridge_set_primary(
    UmiWorkbenchContextLinkAccountBridge *record,
    const char *value);
/**
 * Provide the workbench context link account bridge set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_account_bridge_set_secondary(
    UmiWorkbenchContextLinkAccountBridge *record,
    const char *value);
/**
 * Provide the workbench context link account bridge touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_account_bridge_touch(
    UmiWorkbenchContextLinkAccountBridge *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_account_bridge_archive_encode(const UmiWorkbenchContextLinkAccountBridge *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_account_bridge_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkAccountBridge *value);

#ifdef __cplusplus
}
#endif

#endif
