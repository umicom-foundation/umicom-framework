/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/preview_editor.h
 *
 * PURPOSE:
 *   Track transient preview-editor state and promotion to a permanent tab.
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
#ifndef UMICOM_EDITOR_WORKBENCH_PREVIEW_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_PREVIEW_EDITOR_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb preview editor data shared with callers of this public contract.
 */
typedef struct UmiEditorWbPreviewEditor { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbPreviewEditor;
/**
 * Initialise editor wb preview editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_preview_editor_init(UmiEditorWbPreviewEditor *state,const char *item_id,bool enabled);
/**
 * Copy editor wb preview editor into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_editor_wb_preview_editor_set(UmiEditorWbPreviewEditor *state,bool enabled);
/**
 * Check that editor wb preview editor satisfies its contract before another service relies
 * on it.
 */
int umi_editor_wb_preview_editor_valid(const UmiEditorWbPreviewEditor *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_preview_editor_archive_encode(const UmiEditorWbPreviewEditor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_preview_editor_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbPreviewEditor *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_preview_editor_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbPreviewEditor *expected, UmiEditorWbPreviewEditor *value);


#ifdef __cplusplus
}
#endif
#endif
