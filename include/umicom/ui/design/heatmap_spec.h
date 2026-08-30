/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/heatmap_spec.h
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_HEATMAP_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_HEATMAP_SPEC_H

#include "umicom/ui/design/types.h"
#include "umicom/ui/design/semantic_role.h"
#include "umicom/ui/design/density.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiDesignHeatmapSpec {
    uint16_t rows;
    uint16_t columns;
    double minimum;
    double maximum;
    int show_labels;
} UmiDesignHeatmapSpec;

/* Initialise the semantic heatmap spec specification. */
UmiStatus umi_design_heatmap_spec_init(UmiDesignHeatmapSpec *spec, uint16_t rows, uint16_t columns, double minimum, double maximum, int show_labels);
/* Return one when the semantic specification is internally consistent. */
int umi_design_heatmap_spec_valid(const UmiDesignHeatmapSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
