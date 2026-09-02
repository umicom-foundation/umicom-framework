/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_tab.h
 *
 * PURPOSE:
 *   Describe presentation state for one editor tab.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_TAB_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_TAB_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor tab data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorTab { char id[UMI_EDITOR_WB_ID_CAPACITY]; char parent_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t active_index; bool active; uint64_t revision; } UmiEditorWbEditorTab;
/**
 * Initialise editor wb editor tab from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_editor_wb_editor_tab_init(UmiEditorWbEditorTab *state,const char *id,const char *parent_id); UmiStatus umi_editor_wb_editor_tab_set_count(UmiEditorWbEditorTab *state,size_t count,size_t active_index); int umi_editor_wb_editor_tab_valid(const UmiEditorWbEditorTab *state);

#ifdef __cplusplus
}
#endif
#endif
