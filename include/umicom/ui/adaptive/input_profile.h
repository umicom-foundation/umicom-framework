/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/input_profile.h
 *
 * PURPOSE:
 *   Describe pointer, touch, keyboard and hover capabilities independently of a toolkit.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_INPUT_PROFILE_H
#define UMICOM_UI_ADAPTIVE_INPUT_PROFILE_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiAdaptiveInputProfile {
    int pointer;
    int touch;
    int keyboard;
    int hover;
    int precise_pointer;
} UmiAdaptiveInputProfile;
/* Initialise an input capability profile from platform discovery. */
UmiStatus umi_adaptive_input_profile_init(UmiAdaptiveInputProfile *profile,
                                          int pointer,
                                          int touch,
                                          int keyboard,
                                          int hover,
                                          int precise_pointer);
/* Select the primary interaction mode used for density and navigation adaptation. */
UmiAdaptiveInputMode umi_adaptive_input_profile_primary(const UmiAdaptiveInputProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
