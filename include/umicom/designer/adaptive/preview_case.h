/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/preview_case.h
 *
 * PURPOSE:
 *   Define one expected responsive preview case for deterministic designer verification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_PREVIEW_CASE_H
#define UMICOM_DESIGNER_ADAPTIVE_PREVIEW_CASE_H
#include "umicom/designer/adaptive/responsive_preview.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDesignerPreviewCase {
    char case_id[UMI_DESIGNER_ADAPTIVE_ID_CAPACITY];
    UmiDesignerResponsivePreview preview;
    UmiDesignSizeClass expected_size_class;
    UmiAdaptiveNavigationPattern expected_navigation;
} UmiDesignerPreviewCase;
/* Initialise one preview case with expected semantic results. */
UmiStatus umi_designer_preview_case_init(UmiDesignerPreviewCase *preview_case,
                                         const char *case_id,
                                         const UmiDesignerResponsivePreview *preview,
                                         UmiDesignSizeClass expected_size_class,
                                         UmiAdaptiveNavigationPattern expected_navigation);

#ifdef __cplusplus
}
#endif
#endif
