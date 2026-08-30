/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/panel_adaptation.h
 *
 * PURPOSE:
 *   Adapt panel presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_PANEL_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_PANEL_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptivePanelDecision { UmiAdaptivePresentation presentation; UmiUiPlacement placement; } UmiAdaptivePanelDecision;
/* Adapt a preferred dock placement to current size constraints. */
UmiAdaptivePanelDecision umi_adaptive_panel_adaptation_resolve(UmiDesignSizeClass size_class, UmiUiPlacement preferred);

#ifdef __cplusplus
}
#endif
#endif
