/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_tab_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt tab presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/tab_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveTabDecision d = umi_adaptive_tab_adaptation_resolve(6U, UMI_DESIGN_SIZE_MEDIUM);
    CHECK(d.visible_tabs == 4U && d.scrollable);
    return 0;
}
