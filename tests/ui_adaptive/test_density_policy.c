/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_density_policy.c
 *
 * PURPOSE:
 *   Validate choose canonical design-system density from viewport and input capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/density_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDesignDensity density;
    CHECK(umi_adaptive_density_policy_resolve(UMI_DESIGN_SIZE_COMPACT, UMI_ADAPTIVE_INPUT_TOUCH, &density) == UMI_STATUS_OK);
    CHECK(density == UMI_DESIGN_DENSITY_TOUCH);
    CHECK(umi_adaptive_density_policy_resolve(UMI_DESIGN_SIZE_WIDE, UMI_ADAPTIVE_INPUT_POINTER, &density) == UMI_STATUS_OK);
    CHECK(density == UMI_DESIGN_DENSITY_COMPACT);
    return 0;
}
