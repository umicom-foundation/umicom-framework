/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/alert_state.h
 *
 * PURPOSE:
 *   Track alert activation, occurrence and acknowledgement state.
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
#ifndef UMICOM_UI_ANALYTICS_ALERT_STATE_H
#define UMICOM_UI_ANALYTICS_ALERT_STATE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsAlertState { int active; int acknowledged; int64_t active_since_ns; uint64_t occurrences; } UmiAnalyticsAlertState;
void umi_analytics_alert_state_init(UmiAnalyticsAlertState *s);
void umi_analytics_alert_state_apply(UmiAnalyticsAlertState *s,int triggered,int64_t now_ns);
void umi_analytics_alert_state_acknowledge(UmiAnalyticsAlertState *s);

#ifdef __cplusplus
}
#endif

#endif
