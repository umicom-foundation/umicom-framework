/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/floating_editor.h
 *
 * PURPOSE:
 *   Move semantic nodes into floating placement and restore them to a dock
 *   region without native window handles.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_FLOATING_EDITOR_H
#define UMICOM_WORKBENCH_DESIGNER_FLOATING_EDITOR_H

#include "umicom/workbench_designer/types.h"
#include "umicom/workbench_layout/document.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_workbench_designer_float_node(UmiWorkbenchLayoutDocument *document, const char *node_id, const char *monitor_id, UmiWorkbenchDesignerRect bounds, int32_t z_order);
UmiStatus umi_workbench_designer_dock_node(UmiWorkbenchLayoutDocument *document, const char *node_id, UmiWorkbenchLayoutDockRegion dock_region, const char *parent_node_id, size_t position);

#ifdef __cplusplus
}
#endif

#endif
