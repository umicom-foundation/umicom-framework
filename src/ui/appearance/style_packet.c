/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/style_packet.c
 *
 * PURPOSE:
 *   Bundle resolved theme, typography, density, scale and motion identities for one renderer update.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/style_packet.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_style_packet_init(UmiAppearanceStylePacket *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->packet_id,sizeof item->packet_id,"packet.default");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"theme.default.dark");
    (void)umi_appearance_copy_text(item->typography_policy_id,sizeof item->typography_policy_id,"typography.default");
    item->density=UMI_DESIGN_DENSITY_STANDARD;
    item->scale=1.0;
    item->revision=1U;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_style_packet_is_valid(const UmiAppearanceStylePacket *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->packet_id) && umi_appearance_id_valid(item->theme_pack_id) && item->scale > 0.0 && item->revision > 0U);
}
