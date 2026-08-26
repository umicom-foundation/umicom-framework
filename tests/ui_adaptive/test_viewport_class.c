/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_viewport_class.c
 *
 * PURPOSE:
 *   Validate bridge adaptive logical viewports to the canonical design-system size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/viewport_class.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
int main(void)
{
    UmiDesignSizeClass size_class;
    UmiAdaptiveViewport viewport = { 390, 844 };
    CHECK(umi_adaptive_viewport_class_resolve(viewport, &size_class) == UMI_STATUS_OK);
    CHECK(size_class == UMI_DESIGN_SIZE_COMPACT);
    return 0;
}
