/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/analytics/tick_generator.h
 *
 * PURPOSE:
 *   Generate deterministic evenly spaced linear axis ticks.
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
#ifndef UMICOM_UI_ANALYTICS_TICK_GENERATOR_H
#define UMICOM_UI_ANALYTICS_TICK_GENERATOR_H

#include "umicom/ui/analytics/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the analytics ticks data shared with callers of this public contract.
 */
typedef struct UmiAnalyticsTicks { double values[UMI_ANALYTICS_MAX_BINS]; size_t count; } UmiAnalyticsTicks;
/**
 * Provide the analytics ticks linear operation used by this module and its client
 * applications.
 */
UmiStatus umi_analytics_ticks_linear(double minimum,double maximum,size_t requested,UmiAnalyticsTicks *out_ticks);

#ifdef __cplusplus
}
#endif

#endif
