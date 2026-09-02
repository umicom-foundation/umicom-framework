/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/multi_cursor_model.h
 *
 * PURPOSE:
 *   Maintain distinct editor cursor positions for multi-cursor editing.
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
#ifndef UMICOM_EDITOR_WORKBENCH_MULTI_CURSOR_MODEL_H
#define UMICOM_EDITOR_WORKBENCH_MULTI_CURSOR_MODEL_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb multi cursor model data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbMultiCursorModel { UmiEditorWbPosition cursors[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; } UmiEditorWbMultiCursorModel;
/**
 * Initialise editor wb multi cursor model from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_multi_cursor_model_init(UmiEditorWbMultiCursorModel *model);
/**
 * Add editor wb multi cursor model only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_editor_wb_multi_cursor_model_add(UmiEditorWbMultiCursorModel *model,UmiEditorWbPosition position);
/**
 * Remove editor wb multi cursor model while keeping the remaining records in a valid and
 * discoverable state.
 */
UmiStatus umi_editor_wb_multi_cursor_model_remove(UmiEditorWbMultiCursorModel *model,UmiEditorWbPosition position);

#ifdef __cplusplus
}
#endif
#endif
