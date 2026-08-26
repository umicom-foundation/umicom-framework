/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/chart_adaptation.h
 *
 * PURPOSE:
 *   Adapt chart presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_CHART_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_CHART_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveChartDecision { int legend_visible; int axis_labels_compact; int32_t minimum_height; } UmiAdaptiveChartDecision;
/* Resolve chart chrome and minimum height for the active viewport class. */
UmiAdaptiveChartDecision umi_adaptive_chart_adaptation_resolve(UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
