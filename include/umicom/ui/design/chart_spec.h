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
#include "umicom/base/value_archive.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named design chart kind values accepted by this public contract.
 */
typedef enum UmiDesignChartKind { UMI_DESIGN_CHART_LINE=1, UMI_DESIGN_CHART_AREA=2, UMI_DESIGN_CHART_BAR=3, UMI_DESIGN_CHART_SCATTER=4, UMI_DESIGN_CHART_CANDLESTICK=5 } UmiDesignChartKind;
/**
 * Represent the design chart spec data shared with callers of this public contract.
 */
typedef struct UmiDesignChartSpec { UmiDesignChartKind kind; uint16_t series_count; int legend; int crosshair; int zoom; int pan; } UmiDesignChartSpec;
/* Initialise a semantic analytical-chart specification. */
UmiStatus umi_design_chart_spec_init(UmiDesignChartSpec *spec, UmiDesignChartKind kind, uint16_t series_count, int legend, int crosshair, int zoom, int pan);
/* Return one when chart type and series cardinality are valid. */
int umi_design_chart_spec_valid(const UmiDesignChartSpec *spec);

/** Transfer this value using the portable format described in value_archive.h.
 * Pass NULL bytes and zero capacity to measure the encoded size. The decoder
 * validates every field before replacing the destination; refused input leaves
 * it unchanged. Restored identifiers and revisions are data, not permission
 * to change a live service. Hosts must review and apply state through its owner.
 * Source and destination storage must be separate. Neither call performs I/O. */
UmiStatus umi_design_chart_spec_archive_encode(const UmiDesignChartSpec *value,
    void *bytes, size_t capacity, size_t *out_size);
UmiStatus umi_design_chart_spec_archive_decode(const void *bytes, size_t byte_count,
    UmiDesignChartSpec *value);

#ifdef __cplusplus
}
#endif

#endif
