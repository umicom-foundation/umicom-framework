/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/navigation_checkpoint.h
 *
 * PURPOSE:
 *   Capture a named source-location checkpoint and its navigation reason.
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
#ifndef UMICOM_EDITOR_WORKBENCH_NAVIGATION_CHECKPOINT_H
#define UMICOM_EDITOR_WORKBENCH_NAVIGATION_CHECKPOINT_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb navigation checkpoint data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbNavigationCheckpoint { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbNavigationCheckpoint;
/**
 * Initialise editor wb navigation checkpoint from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_navigation_checkpoint_init(UmiEditorWbNavigationCheckpoint *state,const char *id,const char *text); UmiStatus umi_editor_wb_navigation_checkpoint_set_values(UmiEditorWbNavigationCheckpoint *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_navigation_checkpoint_valid(const UmiEditorWbNavigationCheckpoint *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_navigation_checkpoint_archive_encode(const UmiEditorWbNavigationCheckpoint *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_navigation_checkpoint_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbNavigationCheckpoint *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorWbNavigationCheckpoint object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_wb_navigation_checkpoint_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorWbNavigationCheckpoint *workspace, UmiEditorWbNavigationCheckpoint *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_navigation_checkpoint_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbNavigationCheckpoint *expected, UmiEditorWbNavigationCheckpoint *value);


#ifdef __cplusplus
}
#endif
#endif
