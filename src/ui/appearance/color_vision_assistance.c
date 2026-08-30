/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/color_vision_assistance.c
 *
 * PURPOSE:
 *   Require non-colour cues when semantic state could otherwise depend on colour perception alone.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/color_vision_assistance.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_color_vision_assistance_init(UmiAppearanceColorVisionAssistance *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->profile_id,sizeof item->profile_id,"color-vision.safe");
    item->require_icons=true;
    item->require_text_labels=true;
    item->avoid_red_green_only=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_color_vision_assistance_is_valid(const UmiAppearanceColorVisionAssistance *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->profile_id) && (item->require_icons || item->require_text_labels || item->require_patterns));
}
