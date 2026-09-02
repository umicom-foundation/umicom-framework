/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/gauge_model.h
 *
 * PURPOSE:
 *   Compose canonical Design System gauge specifications with normalized value semantics.
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
#ifndef UMICOM_UI_ANALYTICS_GAUGE_MODEL_H
#define UMICOM_UI_ANALYTICS_GAUGE_MODEL_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "umicom/ui/design/gauge_spec.h"
/**
 * Represent the analytics gauge model data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsGaugeModel { char id[UMI_ANALYTICS_ID_CAPACITY]; UmiDesignGaugeSpec spec; } UmiAnalyticsGaugeModel;
/**
 * Initialise analytics gauge model from caller-provided values so later operations receive
 * a known state.
 */
UmiStatus umi_analytics_gauge_model_init(UmiAnalyticsGaugeModel *g,const char *id,double minimum,double maximum,double value);
/**
 * Provide the analytics gauge model normalized operation used by this module and its
 * client applications.
 */
UmiStatus umi_analytics_gauge_model_normalized(const UmiAnalyticsGaugeModel *g,double *out_value);

#ifdef __cplusplus
}
#endif

#endif
