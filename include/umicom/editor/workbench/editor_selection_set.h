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

typedef struct UmiEditorWbEditorSelectionSet { UmiEditorWbRange ranges[UMI_EDITOR_WB_MAX_SEGMENTS]; size_t count; size_t primary_index; } UmiEditorWbEditorSelectionSet;
void umi_editor_wb_editor_selection_set_init(UmiEditorWbEditorSelectionSet *set);
UmiStatus umi_editor_wb_editor_selection_set_add(UmiEditorWbEditorSelectionSet *set,UmiEditorWbRange range,bool primary);
void umi_editor_wb_editor_selection_set_clear(UmiEditorWbEditorSelectionSet *set);

#ifdef __cplusplus
}
#endif
#endif
