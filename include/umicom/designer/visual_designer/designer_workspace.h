/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/visual_designer/designer_workspace.h
 *
 * PURPOSE:
 *   Represent the complete visual designer workspace selection and dirty state.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DESIGNER_VISUAL_DESIGNER_DESIGNER_WORKSPACE_H
#define UMICOM_DESIGNER_VISUAL_DESIGNER_DESIGNER_WORKSPACE_H
#include "umicom/designer/visual_designer/types.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Represent the visual designer workspace data shared with callers of this public contract.
 */
typedef struct UmiRadDesignerWorkspace {
    char workspace_id[UMI_RAD_ID_CAPACITY];
    char document_id[UMI_RAD_ID_CAPACITY];
    char active_surface_id[UMI_RAD_ID_CAPACITY];
    size_t selection_count;
    bool dirty;
} UmiRadDesignerWorkspace;
/**
 * Initialise visual designer workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_designer_workspace_init(UmiRadDesignerWorkspace *item);
/**
 * Check that visual designer workspace satisfies its contract before another service relies
 * on it.
 */
int umi_rad_designer_workspace_is_valid(const UmiRadDesignerWorkspace *item);
#ifdef __cplusplus
}
#endif
#endif
