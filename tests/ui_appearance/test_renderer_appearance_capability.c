/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_renderer_appearance_capability.c
 *
 * PURPOSE:
 *   Verify declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_appearance_capability.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceRendererAppearanceCapability item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_renderer_appearance_capability_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_renderer_appearance_capability_is_valid(&item)) return 2;
    return 0;
}
