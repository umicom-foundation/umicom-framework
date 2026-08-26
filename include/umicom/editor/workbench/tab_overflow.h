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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_TAB_OVERFLOW_H
#define UMICOM_EDITOR_WORKBENCH_TAB_OVERFLOW_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbTabOverflow { size_t total_tabs; size_t visible_tabs; size_t overflow_tabs; int32_t available_width; int32_t preferred_tab_width; } UmiEditorWbTabOverflow;
UmiStatus umi_editor_wb_tab_overflow_resolve(UmiEditorWbTabOverflow *state,size_t total_tabs,int32_t available_width,int32_t preferred_tab_width);

#ifdef __cplusplus
}
#endif
#endif
