/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/editor_group.h
 *
 * PURPOSE:
 *   Describe one editor group that hosts a tab collection.
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
#ifndef UMICOM_EDITOR_WORKBENCH_EDITOR_GROUP_H
#define UMICOM_EDITOR_WORKBENCH_EDITOR_GROUP_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb editor group data shared with callers of this public contract.
 */
typedef struct UmiEditorWbEditorGroup { char id[UMI_EDITOR_WB_ID_CAPACITY]; char parent_id[UMI_EDITOR_WB_ID_CAPACITY]; size_t item_count; size_t active_index; bool active; uint64_t revision; } UmiEditorWbEditorGroup;
/**
 * Initialise editor wb editor group from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_editor_wb_editor_group_init(UmiEditorWbEditorGroup *state,const char *id,const char *parent_id); UmiStatus umi_editor_wb_editor_group_set_count(UmiEditorWbEditorGroup *state,size_t count,size_t active_index); int umi_editor_wb_editor_group_valid(const UmiEditorWbEditorGroup *state);

#ifdef __cplusplus
}
#endif
#endif
