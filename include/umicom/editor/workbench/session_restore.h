/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/session_restore.h
 *
 * PURPOSE:
 *   Build a bounded editor-session restore plan including skipped missing resources.
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
#ifndef UMICOM_EDITOR_WORKBENCH_SESSION_RESTORE_H
#define UMICOM_EDITOR_WORKBENCH_SESSION_RESTORE_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb session restore data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbSessionRestore { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbSessionRestore;
/**
 * Initialise editor wb session restore from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_session_restore_init(UmiEditorWbSessionRestore *state,const char *id,const char *text); UmiStatus umi_editor_wb_session_restore_set_values(UmiEditorWbSessionRestore *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_session_restore_valid(const UmiEditorWbSessionRestore *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_session_restore_archive_encode(const UmiEditorWbSessionRestore *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_session_restore_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbSessionRestore *value);

#ifdef __cplusplus
}
#endif
#endif
