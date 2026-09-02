/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/tab_editor.h
 *
 * PURPOSE:
 *   Create tab groups, reorder tabs and select the active semantic child.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_TAB_EDITOR_H
#define UMICOM_WORKBENCH_DESIGNER_TAB_EDITOR_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the workbench designer tab wrap node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_tab_wrap_node(UmiWorkbenchLayoutDocument *document, const char *target_node_id, const UmiWorkbenchLayoutNode *new_node, bool new_node_after, char *out_tab_group_id, size_t out_capacity);
/**
 * Provide the workbench designer tab reorder operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_tab_reorder(UmiWorkbenchLayoutDocument *document, const char *tab_group_id, const char *child_node_id, size_t new_position);
/**
 * Provide the workbench designer tab activate operation used by this module and its client
 * applications.
 */
UmiStatus umi_workbench_designer_tab_activate(UmiWorkbenchLayoutDocument *document, const char *tab_group_id, const char *child_node_id);

#ifdef __cplusplus
}
#endif

#endif
