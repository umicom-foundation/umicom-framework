/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/debug.h
 *
 * PURPOSE:
 *   Define command-to-Run and Debug activation rules.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_DEBUG_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_DEBUG_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_debug(void);
#ifdef __cplusplus
}
#endif
#endif
