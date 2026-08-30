/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/dialog_adaptation.c
 *
 * PURPOSE:
 *   Adapt dialog presentation to canonical responsive size classes.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/dialog_adaptation.h"

/* Use full-screen compact dialogs while preserving explicit modal semantics. */
UmiAdaptiveDialogDecision umi_adaptive_dialog_adaptation_resolve(UmiDesignSizeClass size_class, int destructive)
{
    UmiAdaptiveDialogDecision decision;
    decision.full_screen = size_class == UMI_DESIGN_SIZE_COMPACT;
    decision.modal = destructive != 0 || size_class <= UMI_DESIGN_SIZE_MEDIUM;
    decision.max_width = size_class == UMI_DESIGN_SIZE_COMPACT ? 0 : 720;
    return decision;
}
