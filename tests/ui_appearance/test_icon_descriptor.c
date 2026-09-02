/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_icon_descriptor.c
 *
 * PURPOSE:
 *   Verify describe a semantic icon identity, directionality and scalable/symbolic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_descriptor.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceIconDescriptor item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_icon_descriptor_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_icon_descriptor_is_valid(&item)) return 2;
    return 0;
}
