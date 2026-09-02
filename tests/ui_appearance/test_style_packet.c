/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_style_packet.c
 *
 * PURPOSE:
 *   Verify bundle resolved theme, typography, density, scale and motion identities for one renderer update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/style_packet.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceStylePacket item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_style_packet_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_style_packet_is_valid(&item)) return 2;
    return 0;
}
