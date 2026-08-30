/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/tab_spec.c
 *
 * PURPOSE:
 *   Define document/tool tab close, pin, dirty-state and accent semantics.
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

#include "umicom/ui/design/tab_spec.h"

#include <string.h>
int umi_design_tab_spec_valid(const UmiDesignTabSpec *spec) { return spec!=NULL && (spec->accent_role>=UMI_DESIGN_ROLE_NEUTRAL && spec->accent_role<=UMI_DESIGN_ROLE_ACCENT) ? 1 : 0; }
UmiStatus umi_design_tab_spec_init(UmiDesignTabSpec *spec, int closable, int pinnable, int dirty, int attention, UmiDesignSemanticRole accent_role)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->closable=closable?1:0;spec->pinnable=pinnable?1:0;spec->dirty=dirty?1:0;spec->attention=attention?1:0;spec->accent_role=accent_role;
    return umi_design_tab_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
