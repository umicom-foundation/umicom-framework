/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_replace_session.h
 *
 * PURPOSE:
 *   Track in-editor replacement text and applied replacement count.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_REPLACE_SESSION_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_REPLACE_SESSION_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor replace session data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorReplaceSession { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbEditorReplaceSession;
/**
 * Initialise editor wb editor replace session from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_editor_replace_session_init(UmiEditorWbEditorReplaceSession *state,const char *id,const char *text); UmiStatus umi_editor_wb_editor_replace_session_set_values(UmiEditorWbEditorReplaceSession *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_editor_replace_session_valid(const UmiEditorWbEditorReplaceSession *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_replace_session_archive_encode(const UmiEditorWbEditorReplaceSession *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_replace_session_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorReplaceSession *value);

#ifdef __cplusplus
}
#endif
#endif
