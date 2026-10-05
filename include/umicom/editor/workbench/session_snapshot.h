/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/session_snapshot.h
 *
 * PURPOSE:
 *   Capture editor-session counts, active identity and revision metadata.
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
#ifndef UMICOM_EDITOR_WORKBENCH_SESSION_SNAPSHOT_H
#define UMICOM_EDITOR_WORKBENCH_SESSION_SNAPSHOT_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb session snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbSessionSnapshot { char active_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t group_count; uint64_t revision; uint64_t fingerprint; } UmiEditorWbSessionSnapshot;
/**
 * Provide the editor wb session snapshot capture operation used by this module and its
 * client applications.
 */
UmiStatus umi_editor_wb_session_snapshot_capture(UmiEditorWbSessionSnapshot *state,const char *active_id,size_t item_count,size_t group_count,uint64_t revision); int umi_editor_wb_session_snapshot_valid(const UmiEditorWbSessionSnapshot *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_session_snapshot_archive_encode(const UmiEditorWbSessionSnapshot *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_session_snapshot_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbSessionSnapshot *value);

#ifdef __cplusplus
}
#endif
#endif
