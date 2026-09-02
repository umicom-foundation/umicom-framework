/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/menu_spec.c
 *
 * PURPOSE:
 *   Define scalable menu presentation, search and overflow semantics.
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

#include "umicom/ui/design/menu_spec.h"

#include <string.h>
/* Check that design menu spec satisfies its contract before another service relies on it. */
int umi_design_menu_spec_valid(const UmiDesignMenuSpec *spec) { return spec!=NULL && (spec->max_visible_items>0U) ? 1 : 0; }
/*
 * Initialise design menu spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_menu_spec_init(UmiDesignMenuSpec *spec, uint16_t max_visible_items, int searchable, int icons, int accelerators)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->max_visible_items=max_visible_items;spec->searchable=searchable?1:0;spec->icons=icons?1:0;spec->accelerators=accelerators?1:0;
    return umi_design_menu_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
