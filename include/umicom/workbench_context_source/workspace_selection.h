/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_source/workspace_selection.h
 *
 * PURPOSE:
 *   Define the reusable workspace selection snapshot contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_SOURCE_WORKSPACE_SELECTION_H
#define UMICOM_WORKBENCH_CONTEXT_SOURCE_WORKSPACE_SELECTION_H
#include "umicom/workbench_context_source/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context source workspace selection data shared with callers of
 * this public contract.
 */
typedef struct UmiWorkbenchContextSourceWorkspaceSelection {
    uint32_t structure_size;
    char record_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char source_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char panel_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char subject_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char group_id[UMI_WORKBENCH_CONTEXT_SOURCE_ID_CAPACITY];
    char label[UMI_WORKBENCH_CONTEXT_SOURCE_TEXT_CAPACITY];
    UmiWorkbenchContextSourceKind source_kind;
    UmiWorkbenchContextSourceTrigger trigger;
    UmiWorkbenchContextSourceState state;
    UmiContextKind context_kind;
    uint64_t flags;
    uint64_t sequence;
    uint64_t timestamp_ms;
    uint64_t revision;
} UmiWorkbenchContextSourceWorkspaceSelection;

/**
 * Initialise workbench context source workspace selection from caller-provided values so
 * later operations receive a known state.
 */
void umi_workbench_context_source_workspace_selection_init(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *record_id);
/**
 * Check that workbench context source workspace selection satisfies its contract before
 * another service relies on it.
 */
UmiStatus umi_workbench_context_source_workspace_selection_validate(
    const UmiWorkbenchContextSourceWorkspaceSelection *record);
/**
 * Provide the workbench context source workspace selection set source operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_workspace_selection_set_source(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *source_id);
/**
 * Provide the workbench context source workspace selection set panel operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_workspace_selection_set_panel(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *panel_id);
/**
 * Provide the workbench context source workspace selection set subject operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_workspace_selection_set_subject(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *subject_id);
/**
 * Provide the workbench context source workspace selection set group operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_workspace_selection_set_group(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *group_id);
/**
 * Provide the workbench context source workspace selection set label operation used by
 * this module and its client applications.
 */
UmiStatus umi_workbench_context_source_workspace_selection_set_label(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    const char *label);
/**
 * Provide the workbench context source workspace selection hash operation used by this
 * module and its client applications.
 */
uint64_t umi_workbench_context_source_workspace_selection_hash(
    const UmiWorkbenchContextSourceWorkspaceSelection *record);
/**
 * Provide the workbench context source workspace selection touch operation used by this
 * module and its client applications.
 */
void umi_workbench_context_source_workspace_selection_touch(
    UmiWorkbenchContextSourceWorkspaceSelection *record,
    uint64_t sequence,
    uint64_t timestamp_ms);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_workbench_context_source_workspace_selection_archive_encode(const UmiWorkbenchContextSourceWorkspaceSelection *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_workbench_context_source_workspace_selection_archive_decode(const void *bytes, size_t byte_count,
    UmiWorkbenchContextSourceWorkspaceSelection *value);

#ifdef __cplusplus
}
#endif
#endif
