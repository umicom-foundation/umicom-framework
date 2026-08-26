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
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/button_spec.h"

#include <string.h>
int umi_design_button_spec_valid(const UmiDesignButtonSpec *spec) { return spec!=NULL && (spec->role>=UMI_DESIGN_ROLE_NEUTRAL && spec->role<=UMI_DESIGN_ROLE_ACCENT && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && (spec->icon_only || spec->label[0]!='\0')) ? 1 : 0; }
UmiStatus umi_design_button_spec_init(UmiDesignButtonSpec *spec, const char *label, UmiDesignSemanticRole role, UmiDesignDensity density, int icon_only, int destructive)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->role=role; spec->density=density; spec->icon_only=icon_only?1:0; spec->destructive=destructive?1:0; if(label!=NULL){ if(umi_design_copy_text(spec->label,sizeof spec->label,label)!=UMI_STATUS_OK)return UMI_STATUS_CAPACITY_EXCEEDED; }
    return umi_design_button_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
