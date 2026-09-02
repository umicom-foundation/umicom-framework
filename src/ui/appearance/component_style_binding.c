/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/component_style_binding.c
 *
 * PURPOSE:
 *   Bind a semantic component and state map to a Framework style identity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/component_style_binding.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_component_style_binding_init(UmiAppearanceComponentStyleBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->component_id,sizeof item->component_id,"button.primary");
    (void)umi_appearance_copy_text(item->style_id,sizeof item->style_id,"style.button.primary");
    (void)umi_appearance_copy_text(item->state_map_id,sizeof item->state_map_id,"states.interactive");
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_component_style_binding_is_valid(const UmiAppearanceComponentStyleBinding *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->component_id) && umi_appearance_id_valid(item->style_id));
}
