/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_adaptive/test_dialog_adaptation.c
 *
 * PURPOSE:
 *   Validate adapt dialog presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/dialog_adaptation.h"
#define CHECK(x) do { if (!(x)) return 1; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiAdaptiveDialogDecision d = umi_adaptive_dialog_adaptation_resolve(UMI_DESIGN_SIZE_COMPACT, 0);
    CHECK(d.full_screen && d.modal);
    return 0;
}
