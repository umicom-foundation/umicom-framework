/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_appearance_conformance.c
 *
 * PURPOSE:
 *   Verify define release-gate requirements for theme, high-DPI, accessibility and frontend appearance parity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_conformance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceAppearanceConformance item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_conformance_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_conformance_is_valid(&item)) return 2;
    return 0;
}
