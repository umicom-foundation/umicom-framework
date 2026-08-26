/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/readonly_document_state.h
 *
 * PURPOSE:
 *   Track read-only state and the reason it cannot be edited.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_READONLY_DOCUMENT_STATE_H
#define UMICOM_EDITOR_WORKBENCH_READONLY_DOCUMENT_STATE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbReadonlyDocumentState { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbReadonlyDocumentState;
UmiStatus umi_editor_wb_readonly_document_state_init(UmiEditorWbReadonlyDocumentState *state,const char *id,const char *text); UmiStatus umi_editor_wb_readonly_document_state_set_values(UmiEditorWbReadonlyDocumentState *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_readonly_document_state_valid(const UmiEditorWbReadonlyDocumentState *state);

#ifdef __cplusplus
}
#endif
#endif
