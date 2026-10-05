/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/pinned_editor.h
 *
 * PURPOSE:
 *   Track whether an editor is pinned against preview-style replacement.
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
#ifndef UMICOM_EDITOR_WORKBENCH_PINNED_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_PINNED_EDITOR_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb pinned editor data shared with callers of this public contract.
 */
typedef struct UmiEditorWbPinnedEditor { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbPinnedEditor;
/**
 * Initialise editor wb pinned editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_pinned_editor_init(UmiEditorWbPinnedEditor *state,const char *item_id,bool enabled);
/**
 * Copy editor wb pinned editor into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_editor_wb_pinned_editor_set(UmiEditorWbPinnedEditor *state,bool enabled);
/**
 * Check that editor wb pinned editor satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_pinned_editor_valid(const UmiEditorWbPinnedEditor *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_pinned_editor_archive_encode(const UmiEditorWbPinnedEditor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_pinned_editor_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbPinnedEditor *value);

#ifdef __cplusplus
}
#endif
#endif
