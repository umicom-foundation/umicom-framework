/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/icon_descriptor.c
 *
 * PURPOSE:
 *   Describe a semantic icon identity, directionality and scalable/symbolic capabilities.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/icon_descriptor.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_icon_descriptor_init(UmiAppearanceIconDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->icon_id,sizeof item->icon_id,"action.save");
    (void)umi_appearance_copy_text(item->semantic_role,sizeof item->semantic_role,"action.save");
    item->scalable=true;
    item->symbolic=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_icon_descriptor_is_valid(const UmiAppearanceIconDescriptor *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->icon_id) && umi_appearance_id_valid(item->semantic_role));
}
