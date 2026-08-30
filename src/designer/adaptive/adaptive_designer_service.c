/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/designer/adaptive/adaptive_designer_service.c
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
#include "umicom/designer/adaptive/adaptive_designer_service.h"

/* Expose one stable readiness snapshot for Studio and other thin visual-authoring frontends. */
UmiStatus umi_designer_adaptive_service_snapshot(const UmiDesignerAdaptiveVariantSet *variants,
                                                  const UmiDesignerPreviewMatrix *previews,
                                                  const UmiDesignerAdaptiveValidation *validation,
                                                  UmiDesignerAdaptiveServiceSnapshot *out_snapshot)
{
    if(variants==NULL||previews==NULL||validation==NULL||out_snapshot==NULL)return UMI_STATUS_INVALID_ARGUMENT;
    out_snapshot->variant_count=variants->count;
    out_snapshot->preview_count=previews->count;
    out_snapshot->blocking_issues=umi_designer_adaptive_validation_blocking_count(validation);
    out_snapshot->publishable=variants->count>0U&&previews->count>0U&&umi_designer_adaptive_validation_publishable(validation);
    return UMI_STATUS_OK;
}
