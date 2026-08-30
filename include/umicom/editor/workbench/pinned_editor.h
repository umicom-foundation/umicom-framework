/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/pinned_editor.h
 *
 * PURPOSE:
 *   Track whether an editor is pinned against preview-style replacement.
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
#ifndef UMICOM_EDITOR_WORKBENCH_PINNED_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_PINNED_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbPinnedEditor { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbPinnedEditor;
UmiStatus umi_editor_wb_pinned_editor_init(UmiEditorWbPinnedEditor *state,const char *item_id,bool enabled);
UmiStatus umi_editor_wb_pinned_editor_set(UmiEditorWbPinnedEditor *state,bool enabled);
int umi_editor_wb_pinned_editor_valid(const UmiEditorWbPinnedEditor *state);

#ifdef __cplusplus
}
#endif
#endif
