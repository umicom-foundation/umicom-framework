/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/application_brand_binding.c
 *
 * PURPOSE:
 *   Bind a thin application identity to Framework-owned brand and theme-pack identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/application_brand_binding.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_application_brand_binding_init(UmiAppearanceApplicationBrandBinding *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->application_id,sizeof item->application_id,"studio");
    (void)umi_appearance_copy_text(item->brand_id,sizeof item->brand_id,"studio.brand");
    (void)umi_appearance_copy_text(item->theme_pack_id,sizeof item->theme_pack_id,"studio.dark");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_application_brand_binding_is_valid(const UmiAppearanceApplicationBrandBinding *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->application_id) && umi_appearance_id_valid(item->brand_id) && umi_appearance_id_valid(item->theme_pack_id));
}
