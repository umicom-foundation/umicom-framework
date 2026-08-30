/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/mnemonic_policy.c
 *
 * PURPOSE:
 *   Govern mnemonic visibility and uniqueness without embedding toolkit accelerator syntax.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/mnemonic_policy.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_mnemonic_policy_init(UmiAppearanceMnemonicPolicy *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->policy_id,sizeof item->policy_id,"mnemonic.default");
    item->show_on_keyboard_intent=true;
    item->unique_within_scope=true;
    item->localised=true;
    item->allow_auto_assignment=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_mnemonic_policy_is_valid(const UmiAppearanceMnemonicPolicy *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->policy_id) && item->unique_within_scope);
}
