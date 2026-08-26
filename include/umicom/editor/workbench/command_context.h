/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/command_context.h
 *
 * PURPOSE:
 *   Describe current editor command context such as writability and selection state.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_COMMAND_CONTEXT_H
#define UMICOM_EDITOR_WORKBENCH_COMMAND_CONTEXT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbCommandContext { bool writable; bool has_selection; bool has_multiple_cursors; bool has_document; } UmiEditorWbCommandContext;
int umi_editor_wb_command_context_can_edit(const UmiEditorWbCommandContext *state); int umi_editor_wb_command_context_can_find(const UmiEditorWbCommandContext *state);

#ifdef __cplusplus
}
#endif
#endif
