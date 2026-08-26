/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/sidebar_adaptation.h
 *
 * PURPOSE:
 *   Adapt sidebar presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_SIDEBAR_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_SIDEBAR_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveSidebarDecision { UmiAdaptivePresentation presentation; int32_t extent; } UmiAdaptiveSidebarDecision;
/* Resolve side-navigation presentation and extent for the active size class. */
UmiAdaptiveSidebarDecision umi_adaptive_sidebar_adaptation_resolve(UmiDesignSizeClass size_class, int32_t preferred_extent);

#ifdef __cplusplus
}
#endif
#endif
