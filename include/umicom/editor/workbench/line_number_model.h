/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/line_number_model.h
 *
 * PURPOSE:
 *   Resolve absolute, relative or hidden line-number labels.
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
#ifndef UMICOM_EDITOR_WORKBENCH_LINE_NUMBER_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_LINE_NUMBER_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb line number model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbLineNumberModel { UmiEditorWbLineNumberMode mode; uint32_t cursor_line; } UmiEditorWbLineNumberModel;
/**
 * Initialise editor wb line number model from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_line_number_model_init(UmiEditorWbLineNumberModel *model,UmiEditorWbLineNumberMode mode,uint32_t cursor_line);
/**
 * Provide the editor wb line number model value operation used by this module and its
 * client applications.
 */
uint32_t umi_editor_wb_line_number_model_value(const UmiEditorWbLineNumberModel *model,uint32_t line);

#ifdef __cplusplus
}
#endif
#endif
