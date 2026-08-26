/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/keyboard_access_policy.c
 *
 * PURPOSE:
 *   Define keyboard operability and transient-surface escape requirements for all applications.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/keyboard_access_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_keyboard_access_policy_init(UmiAppearanceKeyboardAccessPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"keyboard.default");
    item->require_all_interactive=true;
    item->require_escape_transients=true;
    item->require_arrow_navigation=true;
    item->forbid_keyboard_traps=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_keyboard_access_policy_is_valid(const UmiAppearanceKeyboardAccessPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->require_all_interactive && item->forbid_keyboard_traps);
}
