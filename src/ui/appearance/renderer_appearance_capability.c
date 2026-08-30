/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/renderer_appearance_capability.c
 *
 * PURPOSE:
 *   Declare appearance capabilities and limitations for GTK4, Qt6, Native Web or headless renderers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/renderer_appearance_capability.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_renderer_appearance_capability_init(UmiAppearanceRendererAppearanceCapability *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->renderer_id,sizeof item->renderer_id,"gtk4");
    item->kind=UMI_APPEARANCE_RENDERER_GTK4;
    item->supports_fractional_scale=true;
    item->supports_high_contrast=true;
    item->supports_reduced_motion=true;
    item->supports_symbolic_icons=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_renderer_appearance_capability_is_valid(const UmiAppearanceRendererAppearanceCapability *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->renderer_id) && item->kind >= UMI_APPEARANCE_RENDERER_GTK4 && item->kind <= UMI_APPEARANCE_RENDERER_HEADLESS);
}
