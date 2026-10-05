/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_close_request.h
 *
 * PURPOSE:
 *   Represent a governed request to close an editor item.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_REQUEST_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_CLOSE_REQUEST_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor close request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorCloseRequest { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorCloseRequest;
/**
 * Initialise editor wb editor close request from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_editor_close_request_init(UmiEditorWbEditorCloseRequest *state,const char *resource,const char *group_id);
/**
 * Check that editor wb editor close request satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_editor_close_request_valid(const UmiEditorWbEditorCloseRequest *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_close_request_archive_encode(const UmiEditorWbEditorCloseRequest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_close_request_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorCloseRequest *value);

#ifdef __cplusplus
}
#endif
#endif
