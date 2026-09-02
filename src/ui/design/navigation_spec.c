/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/navigation_spec.c
 *
 * PURPOSE:
 *   Define rail, sidebar and bottom-navigation semantics across desktop, web and mobile compositions.
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

#include "umicom/ui/design/navigation_spec.h"

#include <string.h>
/*
 * Check that design navigation spec satisfies its contract before another service relies
 * on it.
 */
int umi_design_navigation_spec_valid(const UmiDesignNavigationSpec *spec) { return spec!=NULL && (spec->placement>=UMI_UI_PLACEMENT_LEFT && spec->placement<=UMI_UI_PLACEMENT_BOTTOM && spec->item_count>0U) ? 1 : 0; }
/*
 * Initialise design navigation spec from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_design_navigation_spec_init(UmiDesignNavigationSpec *spec, UmiUiPlacement placement, uint16_t item_count, int collapsible, int show_labels)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->placement=placement;spec->item_count=item_count;spec->collapsible=collapsible?1:0;spec->show_labels=show_labels?1:0;
    return umi_design_navigation_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
