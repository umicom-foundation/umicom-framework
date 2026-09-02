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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_EDITOR_WORKBENCH_TAB_SORT_H
#define UMICOM_EDITOR_WORKBENCH_TAB_SORT_H

#include "umicom/editor/workbench/types.h"
#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the editor wb tab sort key data shared with callers of this public contract.
 */
typedef struct UmiEditorWbTabSortKey { bool pinned; bool dirty; char label[UMI_EDITOR_WB_TEXT_CAPACITY]; } UmiEditorWbTabSortKey;
/**
 * Provide the editor wb tab sort compare operation used by this module and its client
 * applications.
 */
int umi_editor_wb_tab_sort_compare(const UmiEditorWbTabSortKey *left,const UmiEditorWbTabSortKey *right);

#ifdef __cplusplus
}
#endif
#endif
