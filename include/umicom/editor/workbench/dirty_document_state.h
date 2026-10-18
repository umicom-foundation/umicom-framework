/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/dirty_document_state.h
 *
 * PURPOSE:
 *   Track dirty and saved revisions for an editor document.
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
#ifndef UMICOM_EDITOR_WORKBENCH_DIRTY_DOCUMENT_STATE_H
#define UMICOM_EDITOR_WORKBENCH_DIRTY_DOCUMENT_STATE_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb dirty document state data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbDirtyDocumentState { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbDirtyDocumentState;
/**
 * Initialise editor wb dirty document state from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_dirty_document_state_init(UmiEditorWbDirtyDocumentState *state,const char *item_id,bool enabled);
/**
 * Copy editor wb dirty document state into module-owned storage so callers keep ownership
 * of their input values.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_editor_wb_dirty_document_state_set(UmiEditorWbDirtyDocumentState *state,bool enabled);
/**
 * Check that editor wb dirty document state satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_dirty_document_state_valid(const UmiEditorWbDirtyDocumentState *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_dirty_document_state_archive_encode(const UmiEditorWbDirtyDocumentState *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_dirty_document_state_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbDirtyDocumentState *value);

/** Decode with caller-owned staging storage rather than a whole-record stack value.
 * workspace and value must each point to a complete, correctly aligned
 * UmiEditorWbDirtyDocumentState object. Input bytes, scratch and destination must
 * not overlap; overlap is INVALID_ARGUMENT. Every refusal leaves value
 * unchanged. Scratch may be changed and must not be used as a live owner.
 * This call allocates nothing and performs no I/O. Serialize access for
 * the complete call; use the domain publication API to accept live state.
 * See umicom/base/value_archive.h for format and workspace ownership. */
UmiStatus umi_editor_wb_dirty_document_state_archive_decode_with_workspace(const void *bytes, size_t byte_count,
    UmiEditorWbDirtyDocumentState *workspace, UmiEditorWbDirtyDocumentState *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_dirty_document_state_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbDirtyDocumentState *expected, UmiEditorWbDirtyDocumentState *value);


#ifdef __cplusplus
}
#endif
#endif
