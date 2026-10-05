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

#ifdef __cplusplus
}
#endif
#endif
