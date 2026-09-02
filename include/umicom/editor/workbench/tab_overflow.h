/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/tab_overflow.h
 *
 * PURPOSE:
 *   Calculate visible and overflowed editor-tab counts for available width.
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
#ifndef UMICOM_EDITOR_WORKBENCH_TAB_OVERFLOW_H
#define UMICOM_EDITOR_WORKBENCH_TAB_OVERFLOW_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb tab overflow data shared with callers of this public contract.
 */
typedef struct UmiEditorWbTabOverflow { size_t total_tabs; size_t visible_tabs; size_t overflow_tabs; int32_t available_width; int32_t preferred_tab_width; } UmiEditorWbTabOverflow;
/**
 * Provide the editor wb tab overflow resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_editor_wb_tab_overflow_resolve(UmiEditorWbTabOverflow *state,size_t total_tabs,int32_t available_width,int32_t preferred_tab_width);

#ifdef __cplusplus
}
#endif
#endif
