/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_responsive_preview.c
 *
 * PURPOSE:
 *   Validate describe one toolkit-neutral responsive preview target for designer rendering.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/responsive_preview.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerResponsivePreview p; UmiAdaptiveViewport v={390,844}; UmiAdaptiveInsets s={44,0,34,0};
    CHECK(umi_designer_responsive_preview_init(&p,"phone",v,s,UMI_ADAPTIVE_INPUT_TOUCH,UMI_RAD_TARGET_NATIVE_WEB)==UMI_STATUS_OK);
    CHECK(p.orientation==UMI_ADAPTIVE_ORIENTATION_PORTRAIT);
    return 0;
}
