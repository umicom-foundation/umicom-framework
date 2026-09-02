/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_design/test_types.c
 *
 * PURPOSE:
 *   Verify shared design-system utilities and bounded metadata handling.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/types.h"

#include <string.h>
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    char text[8]; double value=0.0;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(text,sizeof text,"panel") != UMI_STATUS_OK) return 1;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(text,"panel") != 0) return 2;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(text,4U,"panel") != UMI_STATUS_CAPACITY_EXCEEDED) return 3;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_clamp(12.0,0.0,10.0,&value) != UMI_STATUS_OK || value != 10.0) return 4;
    return umi_design_hash_text("Umicom") != 0U ? 0 : 5;
}
