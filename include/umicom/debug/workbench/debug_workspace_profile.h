/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/debug/workbench/debug_workspace_profile.h
 *
 * PURPOSE:
 *   Persist per-workspace debugger layout and presentation preferences.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral capability orchestrates canonical Debug Service/DAP
 *   runtime state; Studio remains a thin frontend and owns no reusable debug
 *   semantics, adapter protocol, breakpoint engine or inspection engine.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DEBUG_WORKBENCH_DEBUG_WORKSPACE_PROFILE_H
#define UMICOM_DEBUG_WORKBENCH_DEBUG_WORKSPACE_PROFILE_H

#include "umicom/debug/workbench/types.h"
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the debug workbench debug workspace profile data shared with callers of this
 * public contract.
 */
typedef struct UmiDebugWorkbenchDebugWorkspaceProfile {
    char workspace_id[UMI_DEBUG_WORKBENCH_ID_CAPACITY];
    UmiDebugWorkbenchViewKind primary_view;
    uint64_t visible_views;
    bool follow_instruction_pointer;
    bool open_console_on_output;
    uint64_t revision;
} UmiDebugWorkbenchDebugWorkspaceProfile;

/**
 * Initialise debug workbench debug workspace profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_init(UmiDebugWorkbenchDebugWorkspaceProfile *model, const char *workspace_id);
/**
 * Provide the debug workbench debug workspace profile set primary view operation used by
 * this module and its client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_primary_view(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view);
/**
 * Provide the debug workbench debug workspace profile set view visible operation used by
 * this module and its client applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_debug_workbench_debug_workspace_profile_set_view_visible(UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view, bool visible);
/**
 * Provide the debug workbench debug workspace profile view visible operation used by this
 * module and its client applications.
 */
int umi_debug_workbench_debug_workspace_profile_view_visible(const UmiDebugWorkbenchDebugWorkspaceProfile *model, UmiDebugWorkbenchViewKind view);
/**
 * Check that debug workbench debug workspace profile satisfies its contract before another
 * service relies on it.
 */
int umi_debug_workbench_debug_workspace_profile_valid(const UmiDebugWorkbenchDebugWorkspaceProfile *model);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_debug_workbench_debug_workspace_profile_archive_encode(const UmiDebugWorkbenchDebugWorkspaceProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_debug_workbench_debug_workspace_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugWorkspaceProfile *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiDebugWorkbenchDebugWorkspaceProfile object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_debug_workbench_debug_workspace_profile_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiDebugWorkbenchDebugWorkspaceProfile *workspace, UmiDebugWorkbenchDebugWorkspaceProfile *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_debug_workbench_debug_workspace_profile_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiDebugWorkbenchDebugWorkspaceProfile *expected, UmiDebugWorkbenchDebugWorkspaceProfile *value);


#ifdef __cplusplus
}
#endif
#endif
