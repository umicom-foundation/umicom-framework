/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/focus_mode.h
 *
 * PURPOSE:
 *   Track focus mode while preserving the previously visible workbench regions.
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
#ifndef UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H
#define UMICOM_EDITOR_WORKBENCH_FOCUS_MODE_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb focus mode data shared with callers of this public contract.
 */
typedef struct UmiEditorWbFocusMode { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbFocusMode;
/**
 * Initialise editor wb focus mode from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_focus_mode_init(UmiEditorWbFocusMode *state,const char *item_id,bool enabled);
/**
 * Copy editor wb focus mode into module-owned storage so callers keep ownership of their
 * input values.
 */
UmiStatus umi_editor_wb_focus_mode_set(UmiEditorWbFocusMode *state,bool enabled);
/**
 * Check that editor wb focus mode satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_focus_mode_valid(const UmiEditorWbFocusMode *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_focus_mode_archive_encode(const UmiEditorWbFocusMode *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_focus_mode_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbFocusMode *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_focus_mode_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbFocusMode *expected, UmiEditorWbFocusMode *value);


#ifdef __cplusplus
}
#endif
#endif
