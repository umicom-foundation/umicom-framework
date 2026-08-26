/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_style_packet.c
 *
 * PURPOSE:
 *   Verify bundle resolved theme, typography, density, scale and motion identities for one renderer update.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/style_packet.h"
int main(void) {
    UmiAppearanceStylePacket item;
    if (umi_appearance_style_packet_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_style_packet_is_valid(&item)) return 2;
    return 0;
}
