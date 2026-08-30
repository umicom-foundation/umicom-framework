/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/theme_scope.c
 *
 * PURPOSE:
 *   Describe the semantic scope at which a theme override is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/theme_scope.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_theme_scope_init(UmiAppearanceThemeScope *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->scope_id,sizeof item->scope_id,"scope.system");
    (void)umi_appearance_copy_text(item->owner_id,sizeof item->owner_id,"system");
    item->scope=UMI_APPEARANCE_SCOPE_SYSTEM;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_theme_scope_is_valid(const UmiAppearanceThemeScope *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->scope_id) && umi_appearance_id_valid(item->owner_id));
}
