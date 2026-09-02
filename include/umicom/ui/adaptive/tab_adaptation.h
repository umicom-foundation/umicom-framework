/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/tab_adaptation.h
 *
 * PURPOSE:
 *   Adapt tab presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_TAB_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_TAB_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the adaptive tab decision data shared with callers of this public contract.
 */
typedef struct UmiAdaptiveTabDecision { size_t visible_tabs; int scrollable; int compact_titles; } UmiAdaptiveTabDecision;
/* Resolve visible tab budget and scrolling behaviour for one size class. */
UmiAdaptiveTabDecision umi_adaptive_tab_adaptation_resolve(size_t tab_count, UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
