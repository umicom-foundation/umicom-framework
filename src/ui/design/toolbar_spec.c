/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/toolbar_spec.c
 *
 * PURPOSE:
 *   Define toolbar orientation, density and overflow behaviour for reusable command surfaces.
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

#include "umicom/ui/design/toolbar_spec.h"

#include <string.h>
/*
 * Check that design toolbar spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_toolbar_spec_valid(const UmiDesignToolbarSpec *spec) { return spec!=NULL && ((spec->orientation==UMI_UI_HORIZONTAL || spec->orientation==UMI_UI_VERTICAL) && spec->density>=UMI_DESIGN_DENSITY_COMPACT && spec->density<=UMI_DESIGN_DENSITY_TOUCH && spec->preferred_items>0U) ? 1 : 0; }
/*
 * Initialise design toolbar spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_toolbar_spec_init(UmiDesignToolbarSpec *spec, UmiUiOrientation orientation, UmiDesignDensity density, uint16_t preferred_items, int overflow_menu)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->orientation=orientation;spec->density=density;spec->preferred_items=preferred_items;spec->overflow_menu=overflow_menu?1:0;
    return umi_design_toolbar_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
