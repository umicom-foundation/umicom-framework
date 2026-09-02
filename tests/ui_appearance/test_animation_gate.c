/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_animation_gate.c
 *
 * PURPOSE:
 *   Verify decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/animation_gate.h"
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiAppearanceAnimationGate item;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_appearance_animation_gate_init(&item) != UMI_STATUS_OK) return 1;
    /* Apply this operation only while the related capability or state is available. */
    if (!umi_appearance_animation_gate_is_valid(&item)) return 2;
    item.reduced_motion=true; item.essential=false; umi_appearance_animation_gate_resolve(&item); /* Apply this operation only while the related capability or state is available. */ if(item.allowed) return 3;
    return 0;
}
