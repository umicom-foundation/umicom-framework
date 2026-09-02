/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_types.c
 *
 * PURPOSE:
 *   Verify stable appearance utilities and bounds.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/types.h"
#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    char text[16]; double scale = 0.0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_copy_text(text, sizeof text, "theme") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text, "theme") != 0) return 2;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (!umi_appearance_id_valid("studio.theme")) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_clamp_scale(4.0, 0.5, 3.0, &scale) != UMI_STATUS_OK || scale != 3.0) return 4;
    return 0;
}
