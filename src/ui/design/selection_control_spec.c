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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/ui/design/selection_control_spec.h"

#include <string.h>
/*
 * Check that design selection control spec satisfies its contract before another service
 * relies on it.
 */
int umi_design_selection_control_spec_valid(const UmiDesignSelectionControlSpec *spec) { return spec!=NULL && ((spec->kind==UMI_UI_COMPONENT_CHECK_BUTTON || spec->kind==UMI_UI_COMPONENT_SWITCH || spec->kind==UMI_UI_COMPONENT_DROP_DOWN) && !(spec->kind==UMI_UI_COMPONENT_SWITCH && spec->multiple)) ? 1 : 0; }
/*
 * Initialise design selection control spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_selection_control_spec_init(UmiDesignSelectionControlSpec *spec, UmiUiComponentKind kind, int selected, int multiple, int tri_state)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->kind=kind;spec->selected=selected?1:0;spec->multiple=multiple?1:0;spec->tri_state=tri_state?1:0;
    return umi_design_selection_control_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
