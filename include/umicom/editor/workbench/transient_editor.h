/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/transient_editor.h
 *
 * PURPOSE:
 *   Track ephemeral editor activity and whether it should remain retained.
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
#ifndef UMICOM_EDITOR_WORKBENCH_TRANSIENT_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_TRANSIENT_EDITOR_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb transient editor data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbTransientEditor { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbTransientEditor;
/**
 * Initialise editor wb transient editor from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_transient_editor_init(UmiEditorWbTransientEditor *state,const char *id,const char *text); UmiStatus umi_editor_wb_transient_editor_set_values(UmiEditorWbTransientEditor *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_transient_editor_valid(const UmiEditorWbTransientEditor *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_transient_editor_archive_encode(const UmiEditorWbTransientEditor *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_transient_editor_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbTransientEditor *value);

#ifdef __cplusplus
}
#endif
#endif
