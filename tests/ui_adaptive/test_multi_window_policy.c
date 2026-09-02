/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_multi_window_policy.c
 *
 * PURPOSE:
 *   Validate govern detachable and multi-window application surfaces by device family and available width.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/multi_window_policy.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveMultiWindowPolicy p = umi_adaptive_multi_window_policy_resolve(UMI_ADAPTIVE_DEVICE_DESKTOP, UMI_DESIGN_SIZE_WIDE);
    CHECK(p.allow_detach && p.maximum_windows == 16U);
    return 0;
}
