/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/grid_adaptation.h
 *
 * PURPOSE:
 *   Adapt grid presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_GRID_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_GRID_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveGridDecision { size_t visible_columns; int horizontal_scroll; int condensed_headers; } UmiAdaptiveGridDecision;
/* Resolve high-density grid presentation without changing the logical data model. */
UmiAdaptiveGridDecision umi_adaptive_grid_adaptation_resolve(size_t column_count, UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
