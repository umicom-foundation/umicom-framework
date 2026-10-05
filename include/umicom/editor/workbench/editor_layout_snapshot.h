/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_layout_snapshot.h
 *
 * PURPOSE:
 *   Capture immutable editor-layout revision metadata for restore/history.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_SNAPSHOT_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_SNAPSHOT_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor layout snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorLayoutSnapshot { char active_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t group_count; uint64_t revision; uint64_t fingerprint; } UmiEditorWbEditorLayoutSnapshot;
/**
 * Provide the editor wb editor layout snapshot capture operation used by this module and
 * its client applications.
 */
UmiStatus umi_editor_wb_editor_layout_snapshot_capture(UmiEditorWbEditorLayoutSnapshot *state,const char *active_id,size_t item_count,size_t group_count,uint64_t revision); int umi_editor_wb_editor_layout_snapshot_valid(const UmiEditorWbEditorLayoutSnapshot *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_layout_snapshot_archive_encode(const UmiEditorWbEditorLayoutSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_layout_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorLayoutSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
