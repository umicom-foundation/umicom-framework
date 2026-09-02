/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/matrix_layout.h
 *
 * PURPOSE:
 *   Resolve heatmap/matrix cell rectangles from logical viewport dimensions.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_MATRIX_LAYOUT_H
#define UMICOM_UI_ANALYTICS_MATRIX_LAYOUT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics matrix rect data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsMatrixRect { double x,y,width,height; } UmiAnalyticsMatrixRect;
/**
 * Provide the analytics matrix layout cell operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_matrix_layout_cell(uint16_t rows,uint16_t columns,uint16_t row,uint16_t column,double width,double height,double gap,UmiAnalyticsMatrixRect *out_rect);

#ifdef __cplusplus
}
#endif

#endif
