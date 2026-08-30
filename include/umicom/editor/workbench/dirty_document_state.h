/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/dirty_document_state.h
 *
 * PURPOSE:
 *   Track dirty and saved revisions for an editor document.
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
#ifndef UMICOM_EDITOR_WORKBENCH_DIRTY_DOCUMENT_STATE_H
#define UMICOM_EDITOR_WORKBENCH_DIRTY_DOCUMENT_STATE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbDirtyDocumentState { char item_id[UMI_EDITOR_WB_ID_CAPACITY]; bool enabled; bool promoted; uint64_t revision; } UmiEditorWbDirtyDocumentState;
UmiStatus umi_editor_wb_dirty_document_state_init(UmiEditorWbDirtyDocumentState *state,const char *item_id,bool enabled);
UmiStatus umi_editor_wb_dirty_document_state_set(UmiEditorWbDirtyDocumentState *state,bool enabled);
int umi_editor_wb_dirty_document_state_valid(const UmiEditorWbDirtyDocumentState *state);

#ifdef __cplusplus
}
#endif
#endif
