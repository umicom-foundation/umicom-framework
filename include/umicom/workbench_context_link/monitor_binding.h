/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/monitor_binding.h
 *
 * PURPOSE:
 *   Define the reusable monitor context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_MONITOR_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_MONITOR_BINDING_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link monitor binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkMonitorBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char monitor_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkMonitorBinding;

/**
 * Initialise workbench context link monitor binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_monitor_binding_init(UmiWorkbenchContextLinkMonitorBinding *record,
                                           const char *identity);
/**
 * Check that workbench context link monitor binding satisfies its contract before another
 * service relies on it.
 */
UmiStatus umi_workbench_context_link_monitor_binding_validate(
    const UmiWorkbenchContextLinkMonitorBinding *record);
/**
 * Copy workbench context link monitor binding into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_monitor_binding_copy(
    UmiWorkbenchContextLinkMonitorBinding *destination,
    const UmiWorkbenchContextLinkMonitorBinding *source);
/**
 * Provide the workbench context link monitor binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_monitor_binding_hash(
    const UmiWorkbenchContextLinkMonitorBinding *record);
/**
 * Provide the workbench context link monitor binding set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_monitor_binding_set_primary(
    UmiWorkbenchContextLinkMonitorBinding *record,
    const char *value);
/**
 * Provide the workbench context link monitor binding set secondary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_monitor_binding_set_secondary(
    UmiWorkbenchContextLinkMonitorBinding *record,
    const char *value);
/**
 * Provide the workbench context link monitor binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_monitor_binding_touch(
    UmiWorkbenchContextLinkMonitorBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_monitor_binding_archive_encode(const UmiWorkbenchContextLinkMonitorBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_monitor_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkMonitorBinding *value);

#ifdef __cplusplus
}
#endif

#endif
