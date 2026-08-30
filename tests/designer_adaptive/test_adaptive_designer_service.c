/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_adaptive_designer_service.c
 *
 * PURPOSE:
 *   Validate aggregate adaptive visual-authoring state, preview coverage and publication readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_designer_service.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveVariantSet v; UmiDesignerPreviewMatrix p; UmiDesignerAdaptiveValidation x={0U,0U,0U,0U}; UmiDesignerAdaptiveServiceSnapshot s;
    umi_designer_adaptive_variant_set_init(&v); umi_designer_preview_matrix_init(&p);
    CHECK(umi_designer_adaptive_service_snapshot(&v,&p,&x,&s)==UMI_STATUS_OK); CHECK(!s.publishable);
    return 0;
}
