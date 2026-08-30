/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/command_set.h
 *
 * PURPOSE:
 *   Maintain a bounded editor command enablement table.
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
#ifndef UMICOM_EDITOR_WORKBENCH_COMMAND_SET_H
#define UMICOM_EDITOR_WORKBENCH_COMMAND_SET_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbCommandSet { char ids[UMI_EDITOR_WB_MAX_ITEMS][UMI_EDITOR_WB_ID_CAPACITY]; size_t count; uint64_t revision; } UmiEditorWbCommandSet;
void umi_editor_wb_command_set_init(UmiEditorWbCommandSet *state);
UmiStatus umi_editor_wb_command_set_add(UmiEditorWbCommandSet *state,const char *id);
int umi_editor_wb_command_set_contains(const UmiEditorWbCommandSet *state,const char *id);
UmiStatus umi_editor_wb_command_set_remove(UmiEditorWbCommandSet *state,const char *id);

#ifdef __cplusplus
}
#endif
#endif
