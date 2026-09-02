/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/designer/adaptive/adaptive_designer_service.h
 *
 * PURPOSE:
 *   Aggregate adaptive visual-authoring state, preview coverage and publication readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_DESIGNER_SERVICE_H
#define UMICOM_DESIGNER_ADAPTIVE_ADAPTIVE_DESIGNER_SERVICE_H
#include "umicom/designer/adaptive/variant_set.h"
#include "umicom/designer/adaptive/preview_matrix.h"
#include "umicom/designer/adaptive/adaptive_validation.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the designer adaptive service snapshot data shared with callers of this public
 * contract.
 */
typedef struct UmiDesignerAdaptiveServiceSnapshot {
    size_t variant_count;
    size_t preview_count;
    uint32_t blocking_issues;
    int publishable;
} UmiDesignerAdaptiveServiceSnapshot;
/* Build aggregate adaptive-designer readiness from variant, preview and validation state. */
UmiStatus umi_designer_adaptive_service_snapshot(const UmiDesignerAdaptiveVariantSet *variants,
                                                  const UmiDesignerPreviewMatrix *previews,
                                                  const UmiDesignerAdaptiveValidation *validation,
                                                  UmiDesignerAdaptiveServiceSnapshot *out_snapshot);

#ifdef __cplusplus
}
#endif
#endif
