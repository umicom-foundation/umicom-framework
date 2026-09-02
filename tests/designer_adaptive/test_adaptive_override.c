/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/designer_adaptive/test_adaptive_override.c
 *
 * PURPOSE:
 *   Validate represent one variant-scoped responsive property override with explicit inheritance mode.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/designer/adaptive/adaptive_override.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignerAdaptiveOverride o;
    CHECK(umi_designer_adaptive_override_init(&o,"phone","visible","false",UMI_DESIGNER_ADAPTIVE_OVERRIDE)==UMI_STATUS_OK);
    CHECK(o.active);
    return 0;
}
