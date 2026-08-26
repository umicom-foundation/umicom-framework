/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_result.h
 *
 * PURPOSE:
 *   Own bounded key/value rows returned by analytical pipelines.
 *
 * ARCHITECTURE:
 *   This toolkit-neutral analytics capability extends canonical Umicom::ui.
 *   Existing Design System chart/gauge/heatmap specs and workstation surfaces
 *   remain authoritative; GTK4, Qt6, Native Web and thin applications render
 *   the same Framework-owned analytics semantics.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_RESULT_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_RESULT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsResultRow { char key[UMI_ANALYTICS_TEXT_CAPACITY]; double value; } UmiAnalyticsResultRow;
typedef struct UmiAnalyticsResult { UmiAnalyticsResultRow rows[UMI_ANALYTICS_MAX_ITEMS]; size_t count; uint64_t revision; } UmiAnalyticsResult;
void umi_analytics_result_init(UmiAnalyticsResult *result);
UmiStatus umi_analytics_result_add(UmiAnalyticsResult *result,const char *key,double value);

#ifdef __cplusplus
}
#endif

#endif
