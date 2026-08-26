/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/preview_result.h
 *
 * PURPOSE:
 *   Record resolved semantic outcomes and issues for one adaptive preview case.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_PREVIEW_RESULT_H
#define UMICOM_DESIGNER_ADAPTIVE_PREVIEW_RESULT_H
#include "umicom/designer/adaptive/preview_case.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPreviewResult {
    char case_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignerAdaptivePreviewStatus status;
    UmiDesignSizeClass resolved_size_class;
    UmiAdaptiveNavigationPattern resolved_navigation;
    uint32_t issue_count;
} UmiDesignerPreviewResult;
/* Evaluate actual size/navigation semantics against one preview case expectation. */
UmiStatus umi_designer_preview_result_evaluate(const UmiDesignerPreviewCase *preview_case,
                                               UmiDesignSizeClass resolved_size_class,
                                               UmiAdaptiveNavigationPattern resolved_navigation,
                                               UmiDesignerPreviewResult *out_result);

#ifdef __cplusplus
}
#endif
#endif
