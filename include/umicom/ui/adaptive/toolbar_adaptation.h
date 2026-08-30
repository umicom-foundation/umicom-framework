/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/toolbar_adaptation.h
 *
 * PURPOSE:
 *   Adapt toolbar presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_TOOLBAR_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_TOOLBAR_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveToolbarDecision { size_t visible_commands; int overflow; int labels_visible; } UmiAdaptiveToolbarDecision;
/* Resolve visible command budget and label presentation for one size class. */
UmiAdaptiveToolbarDecision umi_adaptive_toolbar_adaptation_resolve(size_t command_count, UmiDesignSizeClass size_class);

#ifdef __cplusplus
}
#endif
#endif
