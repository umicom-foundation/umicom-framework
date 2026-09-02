/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/font_family_descriptor.c
 *
 * PURPOSE:
 *   Describe one semantic font-family candidate and its broad typographic classification.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/font_family_descriptor.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_font_family_descriptor_init(UmiAppearanceFontFamilyDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->family_id,sizeof item->family_id,"font.ui");
    (void)umi_appearance_copy_text(item->family_name,sizeof item->family_name,"system-ui");
    (void)umi_appearance_copy_text(item->classification,sizeof item->classification,"sans");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_font_family_descriptor_is_valid(const UmiAppearanceFontFamilyDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->family_id) && item->family_name[0] != 0);
}
