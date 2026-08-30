/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/category_value.h
 *
 * PURPOSE:
 *   Pair a bounded category label with a finite numeric value.
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
#ifndef UMICOM_UI_ANALYTICS_CATEGORY_VALUE_H
#define UMICOM_UI_ANALYTICS_CATEGORY_VALUE_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAnalyticsCategoryValue { char category[UMI_ANALYTICS_TEXT_CAPACITY]; double value; } UmiAnalyticsCategoryValue;
UmiStatus umi_analytics_category_value_init(UmiAnalyticsCategoryValue *item, const char *category, double value);

#ifdef __cplusplus
}
#endif

#endif
