/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_open_request.h
 *
 * PURPOSE:
 *   Represent a governed request to open a resource in an editor group.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_OPEN_REQUEST_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_OPEN_REQUEST_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor open request data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorOpenRequest { char resource[UMI_EDITOR_WB_PATH_CAPACITY]; char group_id[UMI_EDITOR_WB_ID_CAPACITY]; UmiEditorWbOpenMode mode; bool force; } UmiEditorWbEditorOpenRequest;
/**
 * Initialise editor wb editor open request from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_open_request_init(UmiEditorWbEditorOpenRequest *state,const char *resource,const char *group_id);
/**
 * Check that editor wb editor open request satisfies its contract before another service
 * relies on it.
 */
int umi_editor_wb_editor_open_request_valid(const UmiEditorWbEditorOpenRequest *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_open_request_archive_encode(const UmiEditorWbEditorOpenRequest *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_open_request_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorOpenRequest *value);

#ifdef __cplusplus
}
#endif
#endif
