/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/breakpoint_designer.c
 *
 * PURPOSE:
 *   Own adaptive breakpoint selection and catalogue editing for visual authoring.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/breakpoint_designer.h"
#include <string.h>

/* Reset authoring state while delegating breakpoint storage to the canonical adaptive catalogue. */
void umi_designer_breakpoint_designer_init(UmiDesignerBreakpointDesigner *designer)
{
    if(designer!=NULL){memset(designer,0,sizeof *designer); umi_adaptive_breakpoint_catalogue_init(&designer->catalogue);}
}
/* Add a canonical breakpoint and make it the active designer preview selection. */
UmiStatus umi_designer_breakpoint_designer_add(UmiDesignerBreakpointDesigner *designer,
                                               const UmiDesignBreakpoint *breakpoint)
{
    UmiStatus status;
    if(designer==NULL||breakpoint==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    status=umi_adaptive_breakpoint_catalogue_add(&designer->catalogue,breakpoint); if(status!=UMI_STATUS_OK)return status;
    status=umi_designer_adaptive_copy_text(designer->selected_breakpoint_id,sizeof designer->selected_breakpoint_id,breakpoint->id);
    if (status != UMI_STATUS_OK) {
        return status;
    }
    ++designer->revision;
    return UMI_STATUS_OK;
}
