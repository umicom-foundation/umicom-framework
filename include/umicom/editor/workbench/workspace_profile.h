/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/workspace_profile.h
 *
 * PURPOSE:
 *   Describe reusable editor-workspace defaults without moving them into Studio.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_WORKSPACE_PROFILE_H
#define UMICOM_EDITOR_WORKBENCH_WORKSPACE_PROFILE_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb workspace profile data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbWorkspaceProfile { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbWorkspaceProfile;
/**
 * Initialise editor wb workspace profile from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_workspace_profile_init(UmiEditorWbWorkspaceProfile *state,const char *id,const char *text); UmiStatus umi_editor_wb_workspace_profile_set_values(UmiEditorWbWorkspaceProfile *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_workspace_profile_valid(const UmiEditorWbWorkspaceProfile *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_workspace_profile_archive_encode(const UmiEditorWbWorkspaceProfile *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_workspace_profile_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbWorkspaceProfile *value);

#ifdef __cplusplus
}
#endif
#endif
