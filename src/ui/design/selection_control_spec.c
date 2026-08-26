/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/selection_control_spec.c
 *
 * PURPOSE:
 *   Define checkbox, radio, switch and drop-down selection semantics.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral design capability extends canonical Umicom::ui.
 *   GTK4, Qt6, Native Web and thin applications consume the same semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/selection_control_spec.h"

#include <string.h>
int umi_design_selection_control_spec_valid(const UmiDesignSelectionControlSpec *spec) { return spec!=NULL && ((spec->kind==UMI_UI_COMPONENT_CHECK_BUTTON || spec->kind==UMI_UI_COMPONENT_SWITCH || spec->kind==UMI_UI_COMPONENT_DROP_DOWN) && !(spec->kind==UMI_UI_COMPONENT_SWITCH && spec->multiple)) ? 1 : 0; }
UmiStatus umi_design_selection_control_spec_init(UmiDesignSelectionControlSpec *spec, UmiUiComponentKind kind, int selected, int multiple, int tri_state)
{
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->kind=kind;spec->selected=selected?1:0;spec->multiple=multiple?1:0;spec->tri_state=tri_state?1:0;
    return umi_design_selection_control_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
