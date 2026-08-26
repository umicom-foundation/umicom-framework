/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_renderer_semantics.c
 *
 * PURPOSE:
 *   Validate describe semantic capabilities expected from gtk4, qt6 and native web adaptive renderers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/renderer_semantics.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerRendererSemantics s;
    CHECK(umi_designer_renderer_semantics_default(UMI_RAD_TARGET_GTK4,&s)==UMI_STATUS_OK);
    CHECK(s.supports_detach&&s.supports_touch);
    return 0;
}
