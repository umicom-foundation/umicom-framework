/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/preview_result.c
 *
 * PURPOSE:
 *   Record resolved semantic outcomes and issues for one adaptive preview case.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/preview_result.h"
#include <string.h>

/* Compare semantic expectations rather than renderer pixels so frontends can share conformance tests. */
UmiStatus umi_designer_preview_result_evaluate(const UmiDesignerPreviewCase *preview_case,
                                               UmiDesignSizeClass resolved_size_class,
                                               UmiAdaptiveNavigationPattern resolved_navigation,
                                               UmiDesignerPreviewResult *out_result)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if(preview_case==NULL||out_result==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_result,0,sizeof *out_result);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(umi_designer_adaptive_copy_text(out_result->case_id,sizeof out_result->case_id,preview_case->case_id)!=UMI_STATUS_OK)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    out_result->resolved_size_class=resolved_size_class;
    out_result->resolved_navigation=resolved_navigation;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(resolved_size_class!=preview_case->expected_size_class)++out_result->issue_count;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if(resolved_navigation!=preview_case->expected_navigation)++out_result->issue_count;
    out_result->status=out_result->issue_count==0U?UMI_DESIGNER_PREVIEW_PASS:UMI_DESIGNER_PREVIEW_FAIL;
    return UMI_STATUS_OK;
}
