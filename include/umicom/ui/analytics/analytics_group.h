/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/analytics_group.h
 *
 * PURPOSE:
 *   Accumulate named group count and finite sum for analytical grouping.
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
#ifndef UMICOM_UI_ANALYTICS_ANALYTICS_GROUP_H
#define UMICOM_UI_ANALYTICS_ANALYTICS_GROUP_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics group data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsGroup { char key[UMI_ANALYTICS_TEXT_CAPACITY]; size_t count; double sum; } UmiAnalyticsGroup;
/**
 * Initialise analytics group from caller-provided values so later operations receive a
 * known state.
 */
UmiStatus umi_analytics_group_init(UmiAnalyticsGroup *g,const char *key);
/**
 * Add analytics group only after its inputs and available capacity have been checked.
 */
UmiStatus umi_analytics_group_add(UmiAnalyticsGroup *g,double value);

#ifdef __cplusplus
}
#endif

#endif
