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
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_RESULT_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_RESULT_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics result row data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsResultRow { char key[UMI_ANALYTICS_TEXT_CAPACITY]; double value; } UmiAnalyticsResultRow;
/**
 * Represent the analytics result data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsResult { UmiAnalyticsResultRow rows[UMI_ANALYTICS_MAX_ITEMS]; size_t count; uint64_t revision; } UmiAnalyticsResult;
/**
 * Initialise analytics result from caller-provided values so later operations receive a
 * known state.
 */
void umi_analytics_result_init(UmiAnalyticsResult *result);
/**
 * Add analytics result only after its inputs and available capacity have been checked.
 */
UmiStatus umi_analytics_result_add(UmiAnalyticsResult *result,const char *key,double value);

#ifdef __cplusplus
}
#endif

#endif
