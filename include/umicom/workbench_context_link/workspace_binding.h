/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_link/workspace_binding.h
 *
 * PURPOSE:
 *   Define the reusable workspace context binding contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H
#define UMICOM_WORKBENCH_CONTEXT_LINK_WORKSPACE_BINDING_H

#include "umicom/workbench_context_link/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context link workspace binding data shared with callers of this
 * public contract.
 */
typedef struct UmiWorkbenchContextLinkWorkspaceBinding {
    uint32_t structure_size;
    char binding_id[UMI_WORKBENCH_CONTEXT_LINK_ID_CAPACITY];
    char workspace_id[UMI_WORKBENCH_CONTEXT_LINK_TEXT_CAPACITY];
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
} UmiWorkbenchContextLinkWorkspaceBinding;

/**
 * Initialise workbench context link workspace binding from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_context_link_workspace_binding_init(UmiWorkbenchContextLinkWorkspaceBinding *record,
                                           const char *identity);
/**
 * Check that workbench context link workspace binding satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_link_workspace_binding_validate(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
/**
 * Copy workbench context link workspace binding into module-owned storage so callers keep
 * ownership of their input values.
 */
UmiStatus umi_workbench_context_link_workspace_binding_copy(
    UmiWorkbenchContextLinkWorkspaceBinding *destination,
    const UmiWorkbenchContextLinkWorkspaceBinding *source);
/**
 * Provide the workbench context link workspace binding hash operation used by this module
 * and its client applications.
 */
uint64_t umi_workbench_context_link_workspace_binding_hash(
    const UmiWorkbenchContextLinkWorkspaceBinding *record);
/**
 * Provide the workbench context link workspace binding set primary operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_binding_set_primary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
/**
 * Provide the workbench context link workspace binding set secondary operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_link_workspace_binding_set_secondary(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    const char *value);
/**
 * Provide the workbench context link workspace binding touch operation used by this module
 * and its client applications.
 */
void umi_workbench_context_link_workspace_binding_touch(
    UmiWorkbenchContextLinkWorkspaceBinding *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_link_workspace_binding_archive_encode(const UmiWorkbenchContextLinkWorkspaceBinding *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_link_workspace_binding_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextLinkWorkspaceBinding *value);

#ifdef __cplusplus
}
#endif

#endif
