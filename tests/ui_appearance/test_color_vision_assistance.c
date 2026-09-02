/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_color_vision_assistance.c
 *
 * PURPOSE:
 *   Verify require non-colour cues when semantic state could otherwise depend on colour perception alone.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/color_vision_assistance.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceColorVisionAssistance item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_color_vision_assistance_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_color_vision_assistance_is_valid(&item)) return 2;
    return 0;
}
