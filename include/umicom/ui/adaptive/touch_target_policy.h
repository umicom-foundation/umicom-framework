/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/ui/adaptive/touch_target_policy.h
 *
 * PURPOSE:
 *   Resolve accessible minimum interactive target dimensions for pointer and touch inputs.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_UI_ADAPTIVE_TOUCH_TARGET_POLICY_H
#define UMICOM_UI_ADAPTIVE_TOUCH_TARGET_POLICY_H
#include "umicom/ui/adaptive/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Return the minimum logical square target size for the active input mode. */
int32_t umi_adaptive_touch_target_policy_minimum(UmiAdaptiveInputMode input_mode,
                                                 UmiDesignDensity density);

#ifdef __cplusplus
}
#endif
#endif
