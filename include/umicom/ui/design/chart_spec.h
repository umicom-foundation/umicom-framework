/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/design/chart_spec.h
 *
 * PURPOSE:
 *   Define common analytical chart presentation and interaction semantics shared by Trader, TMS, finance and dashboards.
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

#ifndef INCLUDE_UMICOM_UI_DESIGN_CHART_SPEC_H
#define INCLUDE_UMICOM_UI_DESIGN_CHART_SPEC_H

#include "umicom/ui/design/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum UmiDesignChartKind { UMI_DESIGN_CHART_LINE=1, UMI_DESIGN_CHART_AREA=2, UMI_DESIGN_CHART_BAR=3, UMI_DESIGN_CHART_SCATTER=4, UMI_DESIGN_CHART_CANDLESTICK=5 } UmiDesignChartKind;
typedef struct UmiDesignChartSpec { UmiDesignChartKind kind; uint16_t series_count; int legend; int crosshair; int zoom; int pan; } UmiDesignChartSpec;
/* Initialise a semantic analytical-chart specification. */
UmiStatus umi_design_chart_spec_init(UmiDesignChartSpec *spec, UmiDesignChartKind kind, uint16_t series_count, int legend, int crosshair, int zoom, int pan);
/* Return one when chart type and series cardinality are valid. */
int umi_design_chart_spec_valid(const UmiDesignChartSpec *spec);

#ifdef __cplusplus
}
#endif

#endif
