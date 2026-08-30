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
int umi_design_badge_spec_valid(const UmiDesignBadgeSpec *spec) { return spec!=NULL && (spec->text[0]!='\0' && spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
UmiStatus umi_design_badge_spec_init(UmiDesignBadgeSpec *spec, const char *text, UmiDesignSemanticRole role, int outlined)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
        if (text == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (umi_design_copy_text(spec->text, sizeof spec->text, text) != UMI_STATUS_OK) return UMI_STATUS_CAPACITY_EXCEEDED;
    spec->role = role;
    spec->outlined = outlined ? 1 : 0;
    return umi_design_badge_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
