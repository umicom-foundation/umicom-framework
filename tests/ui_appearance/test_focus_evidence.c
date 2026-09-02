/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_focus_evidence.c
 *
 * PURPOSE:
 *   Verify record keyboard reachability and visible-focus evidence for a semantic interactive element.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/focus_evidence.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceFocusEvidence item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_focus_evidence_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_focus_evidence_is_valid(&item)) return 2;
    return 0;
}
