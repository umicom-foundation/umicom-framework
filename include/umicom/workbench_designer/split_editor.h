/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/split_editor.h
 *
 * PURPOSE:
 *   Create and resize semantic split containers while preserving parent-child
 *   reciprocity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SPLIT_EDITOR_H
#define UMICOM_WORKBENCH_DESIGNER_SPLIT_EDITOR_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Provide the workbench designer split wrap node operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_split_wrap_node(UmiWorkbenchLayoutDocument *document, const char *target_node_id, const UmiWorkbenchLayoutNode *new_node, UmiWorkbenchLayoutOrientation orientation, double ratio, bool new_node_after, char *out_split_node_id, size_t out_capacity);
/**
 * Provide the workbench designer split set ratio operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_split_set_ratio(UmiWorkbenchLayoutDocument *document, const char *split_node_id, double ratio);

#ifdef __cplusplus
}
#endif

#endif
