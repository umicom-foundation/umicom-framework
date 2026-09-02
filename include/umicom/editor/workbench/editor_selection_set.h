/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_selection_set.h
 *
 * PURPOSE:
 *   Maintain a bounded set of editor selections with one primary selection.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_SELECTION_SET_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_SELECTION_SET_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor selection set data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbEditorSelectionSet { UmiEditorWbRange ranges[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t primary_index; } UmiEditorWbEditorSelectionSet;
/**
 * Initialise editor wb editor selection set from caller-provided values so later
 * operations receive a known state.
 */
void umi_editor_wb_editor_selection_set_init(UmiEditorWbEditorSelectionSet *set);
/**
 * Add editor wb editor selection set only after its inputs and available capacity have
 * been checked.
 */
UmiStatus umi_editor_wb_editor_selection_set_add(UmiEditorWbEditorSelectionSet *set,UmiEditorWbRange range,bool primary);
/**
 * Release or reset state held by editor wb editor selection set so the same storage can be
 * reused safely.
 */
void umi_editor_wb_editor_selection_set_clear(UmiEditorWbEditorSelectionSet *set);

#ifdef __cplusplus
}
#endif
#endif
