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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_READONLY_DOCUMENT_STATE_H
#define UMICOM_EDITOR_WORKBENCH_READONLY_DOCUMENT_STATE_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb readonly document state data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbReadonlyDocumentState { char id[UMI_EDITOR_WB_ID_CAPACITY]; char text[UMI_EDITOR_WB_TEXT_CAPACITY]; uint64_t primary; uint64_t secondary; bool enabled; } UmiEditorWbReadonlyDocumentState;
/**
 * Initialise editor wb readonly document state from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_editor_wb_readonly_document_state_init(UmiEditorWbReadonlyDocumentState *state,const char *id,const char *text); UmiStatus umi_editor_wb_readonly_document_state_set_values(UmiEditorWbReadonlyDocumentState *state,uint64_t primary,uint64_t secondary,bool enabled); int umi_editor_wb_readonly_document_state_valid(const UmiEditorWbReadonlyDocumentState *state);

#ifdef __cplusplus
}
#endif
#endif
