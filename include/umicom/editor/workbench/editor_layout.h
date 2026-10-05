/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_layout.h
 *
 * PURPOSE:
 *   Describe a named editor-area layout and its group membership.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_H

#include "umicom/editor/workbench/types.h"
#include "umicom/base/value_archive.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor layout data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorLayout { char id[UMI_EDITOR_WB_ID_CAPACITY]; char parent_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t active_index; bool active; uint64_t revision; } UmiEditorWbEditorLayout;
/**
 * Initialise editor wb editor layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_layout_init(UmiEditorWbEditorLayout *state,const char *id,const char *parent_id); UmiStatus umi_editor_wb_editor_layout_set_count(UmiEditorWbEditorLayout *state,size_t count,size_t active_index); int umi_editor_wb_editor_layout_valid(const UmiEditorWbEditorLayout *state);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_editor_wb_editor_layout_archive_encode(const UmiEditorWbEditorLayout *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_editor_wb_editor_layout_archive_decode(const void *bytes, size_t byte_count,
    UmiEditorWbEditorLayout *value);

#ifdef __cplusplus
}
#endif
#endif
