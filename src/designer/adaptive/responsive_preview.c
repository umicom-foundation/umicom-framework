/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/responsive_preview.c
 *
 * PURPOSE:
 *   Describe one toolkit-neutral responsive preview target for designer rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_preview.h"
#include <string.h>

/* Capture renderer-neutral preview geometry while deriving orientation from the logical viewport. */
UmiStatus umi_designer_responsive_preview_init(UmiDesignerResponsivePreview *preview,
                                               const char *preview_id,
                                               UmiAdaptiveViewport viewport,
                                               UmiAdaptiveInsets safe_area,
                                               UmiAdaptiveInputMode input_mode,
                                               UmiRadTargetKind renderer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(preview==NULL||preview_id==NULL||viewport.width<=0||viewport.height<=0)return UMI_STATUS_INVALID_ARGUMENT;
    memset(preview,0,sizeof *preview);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_designer_adaptive_copy_text(preview->preview_id,sizeof preview->preview_id,preview_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    preview->viewport=viewport;
    preview->safe_area=safe_area;
    preview->input_mode=input_mode;
    preview->renderer=renderer;
    preview->orientation=umi_adaptive_orientation_from_viewport(viewport);
    return UMI_STATUS_OK;
}
