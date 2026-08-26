/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/system_appearance_state.c
 *
 * PURPOSE:
 *   Represent operating-system appearance signals without coupling Framework logic to platform APIs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/system_appearance_state.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_system_appearance_state_init(UmiAppearanceSystemAppearanceState *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->system_id,sizeof item->system_id,"system");
    item->dark_mode=true;
    item->dpi=96U;
    item->scale=1.0;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_system_appearance_state_is_valid(const UmiAppearanceSystemAppearanceState *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->system_id) && item->dpi > 0U && item->scale > 0.0);
}
