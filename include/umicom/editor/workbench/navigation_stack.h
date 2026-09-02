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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_NAVIGATION_STACK_H
#define UMICOM_EDITOR_WORKBENCH_NAVIGATION_STACK_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/editor/workbench/editor_location.h"
/**
 * Represent the editor wb navigation stack data shared with callers of this public
 * contract.
 */
typedef struct UmiEditorWbNavigationStack { UmiEditorWbEditorLocation entries[UMI_EDITOR_WB_MAX_ITEMS]; size_t count; size_t cursor; } UmiEditorWbNavigationStack;
/**
 * Initialise editor wb navigation stack from caller-provided values so later operations
 * receive a known state.
 */
void umi_editor_wb_navigation_stack_init(UmiEditorWbNavigationStack *stack);
/**
 * Provide the editor wb navigation stack push operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_navigation_stack_push(UmiEditorWbNavigationStack *stack,const UmiEditorWbEditorLocation *location);
/**
 * Provide the editor wb navigation stack back operation used by this module and its client
 * applications.
 */
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_back(UmiEditorWbNavigationStack *stack);
/**
 * Provide the editor wb navigation stack forward operation used by this module and its
 * client applications.
 */
const UmiEditorWbEditorLocation *umi_editor_wb_navigation_stack_forward(UmiEditorWbNavigationStack *stack);

#ifdef __cplusplus
}
#endif
#endif
