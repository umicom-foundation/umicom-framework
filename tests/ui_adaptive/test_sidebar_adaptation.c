/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_sidebar_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt sidebar presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/sidebar_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    CHECK(umi_adaptive_sidebar_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT, 300).presentation == UMI_ADAPTIVE_PRESENTATION_OVERLAY);
    return 0;
}
