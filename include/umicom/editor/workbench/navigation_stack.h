/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/navigation_stack.h
 *
 * PURPOSE:
 *   Maintain back/forward source navigation state without replacing canonical navigation history.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_NAVIGATION_STACK_H
#define UMICOM_EDITOR_WORKBENCH_NAVIGATION_STACK_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/editor/workbench/editor_location.h"
typedef struct UmiEditorWbNavigationStack { UmiEditorWbEditorLocation entries[UMI_EDITOR_WB_MAX_ITEMS]; size_t count; size_t cursor; } UmiEditorWbNavigationStack;
void umi_editor_wb_navigation_stack_init(UmiEditorWbNavigationStack *stack);
UmiStatus umi_editor_wb_navigation_stack_push(UmiEditorWbNavigationStack *stack,const UmiEditorWbEditorLocation *location);
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_back(UmiEditorWbNavigationStack *stack);
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_forward(UmiEditorWbNavigationStack *stack);

#ifdef __cplusplus
}
#endif
#endif
