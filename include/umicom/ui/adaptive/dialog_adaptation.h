/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/dialog_adaptation.h
 *
 * PURPOSE:
 *   Adapt dialog presentation to canonical responsive size classes.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_DIALOG_ADAPTATION_H
#define UMICOM_UI_ADAPTIVE_DIALOG_ADAPTATION_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveDialogDecision { int full_screen; int modal; int32_t max_width; } UmiAdaptiveDialogDecision;
/* Resolve dialog modality and width for a size class. */
UmiAdaptiveDialogDecision umi_adaptive_dialog_adaptation_resolve(UmiDesignSizeClass size_class, int destructive);

#ifdef __cplusplus
}
#endif
#endif
