/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/preview_case.c
 *
 * PURPOSE:
 *   Define one expected responsive preview case for deterministic designer verification.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/preview_case.h"
#include <string.h>

/* Bind a preview target to expected size/navigation semantics for repeatable tests. */
UmiStatus umi_designer_preview_case_init(UmiDesignerPreviewCase *preview_case,
                                         const char *case_id,
                                         const UmiDesignerResponsivePreview *preview,
                                         UmiDesignSizeClass expected_size_class,
                                         UmiAdaptiveNavigationPattern expected_navigation)
{
    if(preview_case==NULL||case_id==NULL||preview==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(preview_case,0,sizeof *preview_case);
    if(umi_designer_adaptive_copy_text(preview_case->case_id,sizeof preview_case->case_id,case_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    preview_case->preview=*preview;
    preview_case->expected_size_class=expected_size_class;
    preview_case->expected_navigation=expected_navigation;
    return UMI_STATUS_OK;
}
