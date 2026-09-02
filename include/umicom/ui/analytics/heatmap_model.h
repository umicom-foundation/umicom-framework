/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/heatmap_model.h
 *
 * PURPOSE:
 *   Own a bounded numeric matrix while reusing canonical Design System heatmap specifications.
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
#ifndef UMICOM_UI_ANALYTICS_HEATMAP_MODEL_H
#define UMICOM_UI_ANALYTICS_HEATMAP_MODEL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/heatmap_spec.h"
/**
 * Represent the analytics heatmap model data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsHeatmapModel { char id[UMI_ANALYTICS_ID_CAPACITY]; UmiDesignHeatmapSpec spec; double values[UMI_ANALYTICS_MAX_CELLS]; uint8_t present[UMI_ANALYTICS_MAX_CELLS]; } UmiAnalyticsHeatmapModel;
/**
 * Initialise analytics heatmap model from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_analytics_heatmap_model_init(UmiAnalyticsHeatmapModel *m,const char *id,uint16_t rows,uint16_t columns,double minimum,double maximum);
/**
 * Copy analytics heatmap model into module-owned storage so callers keep ownership of
 * their input values.
 */
UmiStatus umi_analytics_heatmap_model_set(UmiAnalyticsHeatmapModel *m,uint16_t row,uint16_t column,double value);
/**
 * Provide the analytics heatmap model get operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_heatmap_model_get(const UmiAnalyticsHeatmapModel *m,uint16_t row,uint16_t column,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
