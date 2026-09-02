/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_layout.h
 *
 * PURPOSE:
 *   Describe a named editor-area layout and its group membership.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_LAYOUT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor layout data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorLayout { char id[UMI_EDITOR_WB_ID_CAPACITY]; char parent_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t active_index; bool active; uint64_t revision; } UmiEditorWbEditorLayout;
/**
 * Initialise editor wb editor layout from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_layout_init(UmiEditorWbEditorLayout *state,const char *id,const char *parent_id); UmiStatus umi_editor_wb_editor_layout_set_count(UmiEditorWbEditorLayout *state,size_t count,size_t active_index); int umi_editor_wb_editor_layout_valid(const UmiEditorWbEditorLayout *state);

#ifdef __cplusplus
}
#endif
#endif
