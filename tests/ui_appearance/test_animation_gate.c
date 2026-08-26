/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ui_appearance/test_animation_gate.c
 *
 * PURPOSE:
 *   Verify decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/animation_gate.h"
int main(void) {
    UmiAppearanceAnimationGate item;
    if (umi_appearance_animation_gate_init(&item) != UMI_STATUS_OK) return 1;
    if (!umi_appearance_animation_gate_is_valid(&item)) return 2;
    item.reduced_motion=true; item.essential=false; umi_appearance_animation_gate_resolve(&item); if(item.allowed) return 3;
    return 0;
}
