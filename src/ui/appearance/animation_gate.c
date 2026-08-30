/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/appearance/animation_gate.c
 *
 * PURPOSE:
 *   Decide whether an animation may run after reduced-motion and essential-feedback policy is applied.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/appearance/animation_gate.h"
#include <string.h>
/* Initialise bounded state without allocating renderer-specific resources. */
UmiStatus umi_appearance_animation_gate_init(UmiAppearanceAnimationGate *item) {
    if (item == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(item, 0, sizeof *item);
    (void)umi_appearance_copy_text(item->animation_id,sizeof item->animation_id,"progress.pulse");
    item->essential=true;
    item->reduced_motion=false;
    item->allowed=true;
    return UMI_STATUS_OK;
}

/* Validate semantic invariants before the record is published to a renderer. */
int umi_appearance_animation_gate_is_valid(const UmiAppearanceAnimationGate *item) {
    if (item == NULL) return 0;
    return (umi_appearance_id_valid(item->animation_id) && (!item->reduced_motion || item->essential || !item->allowed));
}
void umi_appearance_animation_gate_resolve(UmiAppearanceAnimationGate *item){if(item!=NULL)item->allowed=(!item->reduced_motion)||item->essential;}
