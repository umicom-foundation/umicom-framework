/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/card_spec.c
 *
 * PURPOSE:
 *   Define reusable card elevation, interaction and semantic intent.
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

#include "umicom/ui/design/card_spec.h"

#include <string.h>
int umi_design_card_spec_valid(const UmiDesignCardSpec *spec) { return spec!=NULL && (spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT && spec->elevation_level<=5U) ? 1 : 0; }
UmiStatus umi_design_card_spec_init(UmiDesignCardSpec *spec, UmiDesignSemanticRole role, uint8_t elevation_level, int interactive, int selected)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->role=role;spec->elevation_level=elevation_level;spec->interactive=interactive?1:0;spec->selected=selected?1:0;
    return umi_design_card_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
