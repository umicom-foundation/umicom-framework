/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/ui/adaptive/input_profile.c
 *
 * PURPOSE:
 *   Describe pointer, touch, keyboard and hover capabilities independently of a toolkit.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/ui/adaptive/input_profile.h"

/* Capture input capabilities with normalised boolean fields. */
UmiStatus umi_adaptive_input_profile_init(UmiAdaptiveInputProfile *profile,
                                          int pointer,
                                          int touch,
                                          int keyboard,
                                          int hover,
                                          int precise_pointer)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    profile->pointer = pointer != 0;
    profile->touch = touch != 0;
    profile->keyboard = keyboard != 0;
    profile->hover = hover != 0;
    profile->precise_pointer = precise_pointer != 0;
    return UMI_STATUS_OK;
}

/* Choose a stable primary mode while preserving hybrid capability in the profile. */
UmiAdaptiveInputMode umi_adaptive_input_profile_primary(const UmiAdaptiveInputProfile *profile)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) {
        return UMI_ADAPTIVE_INPUT_POINTER;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->touch != 0 && profile->pointer != 0) {
        return UMI_ADAPTIVE_INPUT_HYBRID;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->touch != 0) {
        return UMI_ADAPTIVE_INPUT_TOUCH;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (profile->pointer != 0) {
        return UMI_ADAPTIVE_INPUT_POINTER;
    }
    return UMI_ADAPTIVE_INPUT_KEYBOARD;
}
