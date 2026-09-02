/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/appearance_conformance.c
 *
 * PURPOSE:
 *   Define release-gate requirements for theme, high-DPI, accessibility and frontend appearance parity.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/appearance_conformance.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_conformance_init(UmiAppearanceAppearanceConformance *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->profile_id,sizeof item->profile_id,"appearance.release");
    item->require_theme_parity=true;
    item->require_high_dpi=true;
    item->require_contrast=true;
    item->require_keyboard_focus=true;
    item->require_reduced_motion=true;
    item->passed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_conformance_is_valid(const UmiAppearanceAppearanceConformance *item) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->profile_id));
}
