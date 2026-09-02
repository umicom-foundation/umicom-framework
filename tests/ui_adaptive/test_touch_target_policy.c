/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_touch_target_policy.c
 *
 * PURPOSE:
 *   Validate resolve accessible minimum interactive target dimensions for pointer and touch inputs.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/touch_target_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    CHECK(umi_adaptive_touch_target_policy_minimum(UMI_ADAPTIVE_INPUT_TOUCH, UMI_DESIGN_DENSITY_STANDARD) == 44);
    CHECK(umi_adaptive_touch_target_policy_minimum(UMI_ADAPTIVE_INPUT_POINTER, UMI_DESIGN_DENSITY_COMPACT) == 28);
    return 0;
}
