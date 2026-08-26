/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_preview_result.c
 *
 * PURPOSE:
 *   Validate record resolved semantic outcomes and issues for one adaptive preview case.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/preview_result.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerResponsivePreview p; UmiDesignerPreviewCase c; UmiDesignerPreviewResult r; UmiAdaptiveViewport v={390,844}; UmiAdaptiveInsets s={0,0,0,0};
    CHECK(umi_designer_responsive_preview_init(&p,"p",v,s,UMI_ADAPTIVE_INPUT_TOUCH,UMI_RAD_TARGET_NATIVE_WEB)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_case_init(&c,"c",&p,UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_NAV_BOTTOM_BAR)==UMI_STATUS_OK);
    CHECK(umi_designer_preview_result_evaluate(&c,UMI_DESIGN_SIZE_COMPACT,UMI_ADAPTIVE_NAV_BOTTOM_BAR,&r)==UMI_STATUS_OK);
    CHECK(r.status==UMI_DESIGNER_PREVIEW_PASS);
    return 0;
}
