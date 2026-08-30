/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/preview_editor.h
 *
 * PURPOSE:
 *   Track transient preview-editor state and promotion to a permanent tab.
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
#ifndef UMICOM_EDITOR_WORKBENCH_PREVIEW_EDITOR_H
#define UMICOM_EDITOR_WORKBENCH_PREVIEW_EDITOR_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbPreviewEditor { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbPreviewEditor;
UmiStatus umi_editor_wb_preview_editor_init(UmiEditorWbPreviewEditor *state,const char *item_id,bool enabled);
UmiStatus umi_editor_wb_preview_editor_set(UmiEditorWbPreviewEditor *state,bool enabled);
int umi_editor_wb_preview_editor_valid(const UmiEditorWbPreviewEditor *state);

#ifdef __cplusplus
}
#endif
#endif
