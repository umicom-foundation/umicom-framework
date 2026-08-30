/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_window_class.c
 *
 * PURPOSE:
 *   Validate describe designer preview window classes and minimum logical dimensions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/window_class.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignerAdaptiveWindowClass c; UmiAdaptiveViewport v={1200,800};
    CHECK(umi_designer_window_class_init(&c,"workstation",1024,700,1)==UMI_STATUS_OK);
    CHECK(umi_designer_window_class_matches(&c,v));
    return 0;
}
