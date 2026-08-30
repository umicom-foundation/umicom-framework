/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/rad/designer_workspace.c
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
#include "umicom/designer/rad/designer_workspace.h"
#include <string.h>
UmiStatus umi_rad_designer_workspace_init(UmiRadDesignerWorkspace *item){
    if(item==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(item,0,sizeof *item);
    (void)umi_rad_copy_text(item->workspace_id, sizeof item->workspace_id, "designer_workspace");
    (void)umi_rad_copy_text(item->document_id, sizeof item->document_id, "designer_workspace");
    (void)umi_rad_copy_text(item->active_surface_id, sizeof item->active_surface_id, "designer_workspace");
    return UMI_STATUS_OK;
}
int umi_rad_designer_workspace_is_valid(const UmiRadDesignerWorkspace *item){if(item==NULL)return 0;return umi_rad_id_valid(item->workspace_id) && umi_rad_id_valid(item->document_id);}
