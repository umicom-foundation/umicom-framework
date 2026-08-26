/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/component_state_projection.c
 *
 * PURPOSE:
 *   Map semantic component state to resolved style and accessibility state identifiers.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_state_projection.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_component_state_projection_init(UmiAppearanceComponentStateProjection *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->component_id,sizeof item->component_id,"button.primary");
    (void)umi_appearance_copy_text(item->state_id,sizeof item->state_id,"normal");
    (void)umi_appearance_copy_text(item->resolved_style_id,sizeof item->resolved_style_id,"style.button.primary.normal");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_component_state_projection_is_valid(const UmiAppearanceComponentStateProjection *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->component_id) && umi_appearance_id_valid(item->state_id) && umi_appearance_id_valid(item->resolved_style_id));
}
