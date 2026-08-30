/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/reference_line.h
 *
 * PURPOSE:
 *   Describe labelled horizontal or vertical analytical reference lines.
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
#ifndef UMICOM_UI_ANALYTICS_REFERENCE_LINE_H
#define UMICOM_UI_ANALYTICS_REFERENCE_LINE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsReferenceLine { char label[UMI_ANALYTICS_TEXT_CAPACITY]; UmiAnalyticsOrientation orientation; double value; } UmiAnalyticsReferenceLine;
UmiStatus umi_analytics_reference_line_init(UmiAnalyticsReferenceLine *item);
int umi_analytics_reference_line_valid(const UmiAnalyticsReferenceLine *item);

#ifdef __cplusplus
}
#endif

#endif
