/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/design/heatmap_spec.c
 *
 * PURPOSE:
 *   Define heatmap matrix dimensions and numeric domain for risk, analytics and diagnostics surfaces.
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

#include "umicom/ui/design/heatmap_spec.h"

#include <string.h>
/*
 * Check that design heatmap spec satisfies its contract before another service relies on
 * it.
 */
int umi_design_heatmap_spec_valid(const UmiDesignHeatmapSpec *spec) { return spec!=NULL && (spec->rows>0U && spec->columns>0U && spec->rows<=128U && spec->columns<=128U && umi_design_number_valid(spec->minimum) && umi_design_number_valid(spec->maximum) && spec->maximum>spec->minimum) ? 1 : 0; }
/*
 * Initialise design heatmap spec from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_design_heatmap_spec_init(UmiDesignHeatmapSpec *spec, uint16_t rows, uint16_t columns, double minimum, double maximum, int show_labels)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (spec==NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(spec,0,sizeof *spec);
    spec->rows=rows;spec->columns=columns;spec->minimum=minimum;spec->maximum=maximum;spec->show_labels=show_labels?1:0;
    return umi_design_heatmap_spec_valid(spec) ? UMI_STATUS_OK : UMI_STATUS_INVALID_ARGUMENT;
}
