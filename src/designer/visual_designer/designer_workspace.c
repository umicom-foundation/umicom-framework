/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/visual_designer/designer_workspace.c
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
#include "umicom/designer/visual_designer/designer_workspace.h"
#include <string.h>
/*
 * Initialise visual designer workspace from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_rad_designer_workspace_init(UmiRadDesignerWorkspace *item){
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->workspace_id, sizeof item->workspace_id, "designer_workspace");
    (void)umi_rad_copy_text(item->document_id, sizeof item->document_id, "designer_workspace");
    (void)umi_rad_copy_text(item->active_surface_id, sizeof item->active_surface_id, "designer_workspace");
    return UMI_STATUS_OK;
}
/*
 * Check that visual designer workspace satisfies its contract before another service relies
 * on it.
 */
int umi_rad_designer_workspace_is_valid(const UmiRadDesignerWorkspace *item){/* Protect caller-owned memory by checking that required state is available before it is used. */ if(item==NULL)return 0;return umi_rad_id_valid(item->workspace_id) && umi_rad_id_valid(item->document_id);}
