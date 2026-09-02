/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_pipeline.h
 *
 * PURPOSE:
 *   Describe ordered analytical processing stages with structural validation.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_PIPELINE_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_PIPELINE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics pipeline data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsPipeline { UmiAnalyticsStageKind stages[16]; size_t count; } UmiAnalyticsPipeline;
/**
 * Initialise analytics pipeline from caller-provided values so later operations receive a
 * known state.
 */
void umi_analytics_pipeline_init(UmiAnalyticsPipeline *pipeline);
/**
 * Add analytics pipeline only after its inputs and available capacity have been checked.
 */
UmiStatus umi_analytics_pipeline_add(UmiAnalyticsPipeline *pipeline,UmiAnalyticsStageKind stage);
/**
 * Check that analytics pipeline satisfies its contract before another service relies on
 * it.
 */
int umi_analytics_pipeline_valid(const UmiAnalyticsPipeline *pipeline);

#ifdef __cplusplus
}
#endif

#endif
