/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/editor/workbench/tab_sort.h
 *
 * PURPOSE:
 *   Provide deterministic editor-tab ordering rules.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral editor-workbench capability extends canonical
 *   Umicom::editor and composes Framework-owned UI semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_TAB_SORT_H
#define UMICOM_EDITOR_WORKBENCH_TAB_SORT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiEditorWbTabSortKey { bool pinned; bool dirty; char label[UMI_EDITOR_WB_TEXT_CAPACITY]; } UmiEditorWbTabSortKey;
int umi_editor_wb_tab_sort_compare(const UmiEditorWbTabSortKey *left,const UmiEditorWbTabSortKey *right);

#ifdef __cplusplus
}
#endif
#endif
