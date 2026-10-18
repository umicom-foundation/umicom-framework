/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_item.h
 *
 * PURPOSE:
 *   Describe one open editor item independently of any toolkit tab widget.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_ITEM_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor item data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorItem { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; char path[UMI_EDITOR_WB_PATH_CAPACITY]; UmiEditorWbOpenMode open_mode; bool dirty; bool pinned; uint64_t revision; } UmiEditorWbEditorItem;
/**
 * Initialise editor wb editor item from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_editor_item_init(UmiEditorWbEditorItem *item,const char *id,const char *path,UmiEditorWbOpenMode mode);
/**
 * Provide the editor wb editor item set dirty operation used by this module and its client
 * applications.
 *
 * @retval UMI_STATUS_CAPACITY_EXCEEDED A required counter is at UINT64_MAX.
 * Checked fields: revision. The complete value is unchanged on this refusal.
 * The last representable value may be reached; counters never wrap to zero.
 */
UmiStatus umi_editor_wb_editor_item_set_dirty(UmiEditorWbEditorItem *item,bool dirty);
/**
 * Check that editor wb editor item satisfies its contract before another service relies on
 * it.
 */
int umi_editor_wb_editor_item_valid(const UmiEditorWbEditorItem *item);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_item_archive_encode(const UmiEditorWbEditorItem *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_item_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorItem *value);

/** Decode only if value's encoded fields still match the expected snapshot.
 * INVALID_STATE reports a changed snapshot. Every failure leaves value intact.
 * Comparison ignores padding and unused text; decoded revisions remain archive
 * evidence. This allocating operation requires owner-thread serialization and
 * grants no execution authority. Use the owner's publication API for live state.
 * See umicom/base/value_archive.h for limits and snapshot ownership. */
UmiStatus umi_editor_wb_editor_item_archive_decode_if_unchanged(const void *bytes, size_t byte_count,
    const UmiEditorWbEditorItem *expected, UmiEditorWbEditorItem *value);


#ifdef __cplusplus
}
#endif
#endif
