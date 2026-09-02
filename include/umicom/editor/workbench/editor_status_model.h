/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_status_model.h
 *
 * PURPOSE:
 *   Expose line, column, selection, language, encoding and dirty status.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_STATUS_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_STATUS_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor status model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorStatusModel { uint32_t line; uint32_t column; size_t selection_length; char language[64]; char encoding[32]; char eol[16]; bool dirty; } UmiEditorWbEditorStatusModel;
/**
 * Initialise editor wb editor status model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_status_model_init(UmiEditorWbEditorStatusModel *state,const char *language,const char *encoding); UmiStatus umi_editor_wb_editor_status_model_set_position(UmiEditorWbEditorStatusModel *state,uint32_t line,uint32_t column);

#ifdef __cplusplus
}
#endif
#endif
