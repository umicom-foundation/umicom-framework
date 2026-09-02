/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/button_spec.c
 *
 * PURPOSE:
 *   Define button intent, size, label and icon semantics independent of frontend toolkit.
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

#include "umicom/ui/design/button_spec.h"

#include <string.h>
/*
 * Check that design button spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_button_spec_valid(const UmiDesignButtonSpec *spec) { return spec!=NULL && (spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && (spec->icon_only || spec->label[0]!='\0')) ? 1 : 0; }
/*
 * Initialise design button spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_button_spec_init(UmiDesignButtonSpec *spec, const char *label, UmiDesignSemanticRole role, UmiDesignDensity density, int icon_only, int destructive)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->role=role; spec->density=density; spec->icon_only=icon_only?1:0; spec->destructive=destructive?1:0; /* Protect caller-owned memory by checking that required state is available before it is used. */ if(label!=NULL){ /* Protect caller-owned memory by checking that required state is available before it is used. */ if(umi_design_copy_text(spec->label,sizeof spec->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; }
    return umi_design_button_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
