/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/chart/drawing_appearance.h
 * PURPOSE: Define portable drawing colours, widths and fill opacity with versioned persistence.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_CHART_DRAWING_APPEARANCE_H
#define UMICOM_CHART_DRAWING_APPEARANCE_H
#include "umicom/chart/drawing_tools.h"
#ifdef __cplusplus
extern "C" {
#endif
/** RGB channels range from 0 to 255; width is 10..80 tenths of a logical
 * pixel; fill is 0..80 percent. Fill is used only for ranges and zones.
 * Custom outlines and level labels are opaque. Tool defaults retain the
 * supplied theme colours, including any theme alpha. */
typedef struct UmiChartDrawingAppearance {
    uint32_t red, green, blue, width_tenths, fill_percent;
} UmiChartDrawingAppearance;
typedef struct UmiChartDrawingResolvedAppearance {
    UmiChartColor outline, fill;
    double width;
} UmiChartDrawingResolvedAppearance;

/** Parse exactly #RRGGBB, case-insensitively, together with the numeric
 * fields. Outputs of all value functions remain unchanged on failure. */
UmiStatus UmiChartDrawingAppearanceFromHex(const char *hex, uint32_t widthTenths,
    uint32_t fillPercent, UmiChartDrawingAppearance *outAppearance);
UmiStatus UmiChartDrawingAppearanceValidate(const UmiChartDrawingAppearance *appearance);
/** Encode the locale-independent form umi-drawing:1:RRGGBB:WW:FF.
 * All fields have fixed widths. Encoding fits in the existing style field. */
UmiStatus UmiChartDrawingAppearanceEncode(const UmiChartDrawingAppearance *appearance,
    char *outText, size_t capacity);
/** Decode a NUL-terminated string within capacity. Empty returns NOT_FOUND,
 * other formats/versions return UNAVAILABLE, malformed version 1 returns
 * PARSE_ERROR. Missing terminator returns INVALID_ARGUMENT. No implicit migration. */
UmiStatus UmiChartDrawingAppearanceDecode(const char *text, size_t capacity,
    UmiChartDrawingAppearance *outAppearance);
/** Resolve known appearance or the exact existing tool/theme defaults.
 * Legacy, unknown and malformed styles remain stored and render with defaults.
 * The caller supplies a valid plot theme. Geometry is validated here. */
UmiStatus UmiChartDrawingAppearanceResolve(const UmiChartDrawingSnapshot *drawing,
    const UmiChartPlotStyle *theme, UmiChartDrawingResolvedAppearance *outAppearance);
/** Owner-thread only. Compare the pane and displayed row revision before
 * publishing a copied style. A geometry lock or hidden state does not forbid
 * appearance changes. A NULL appearance explicitly clears to tool defaults.
 * This is the only clearing operation; decoding and rendering never rewrite
 * legacy/unknown style bytes. A no-op retains registry and row revisions. */
UmiStatus UmiChartDrawingSetAppearance(UmiChartDrawingRegistry *registry,
    const char *pane, const char *id, uint64_t expectedRevision,
    const UmiChartDrawingAppearance *appearance);
#ifdef __cplusplus
}
#endif
#endif
