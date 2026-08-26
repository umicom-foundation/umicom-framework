/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/shortcut_presentation.c
 *
 * PURPOSE:
 *   Describe platform-neutral command shortcut hints for menus, toolbars and palettes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/shortcut_presentation.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_shortcut_presentation_init(UmiAppearanceShortcutPresentation *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->action_id,sizeof item->action_id,"file.save");
    (void)umi_appearance_copy_text(item->accelerator_id,sizeof item->accelerator_id,"primary+s");
    (void)umi_appearance_copy_text(item->display_text,sizeof item->display_text,"Save");
    item->discoverable=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_shortcut_presentation_is_valid(const UmiAppearanceShortcutPresentation *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->action_id) && umi_appearance_id_valid(item->accelerator_id));
}
