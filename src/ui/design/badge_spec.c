/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/badge_spec.c
 *
 * PURPOSE:
 *   Define compact status and metadata badges with semantic intent.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/badge_spec.h"

#include <string.h>
/* Check that design badge spec satisfies its contract before another service relies on it. */
int umi_design_badge_spec_valid(const UmiDesignBadgeSpec *spec) { return spec!=NULL && (spec->text[0]!='\0' && spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
/*
 * Initialise design badge spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_badge_spec_init(UmiDesignBadgeSpec *spec, const char *text, UmiDesignSemanticRole role, int outlined)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_design_copy_text(spec->text, sizeof spec->text, text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->role = role;
    spec->outlined = outlined ? 1 : 0;
    return umi_design_badge_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
