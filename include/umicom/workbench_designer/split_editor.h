/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/split_editor.h
 *
 * PURPOSE:
 *   Create and resize semantic split containers while preserving parent-child
 *   reciprocity.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_SPLIT_EDITOR_H
#define UMICOM_WORKBENCH_DESIGNER_SPLIT_EDITOR_H

#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_workbench_designer_split_wrap_node(UmiWorkbenchLayoutDocument *document, const char *target_node_id, const UmiWorkbenchLayoutNode *new_node, UmiWorkbenchLayoutOrientation orientation, double ratio, bool new_node_after, char *out_split_node_id, size_t out_capacity);
UmiStatus umi_workbench_designer_split_set_ratio(UmiWorkbenchLayoutDocument *document, const char *split_node_id, double ratio);

#ifdef __cplusplus
}
#endif

#endif
