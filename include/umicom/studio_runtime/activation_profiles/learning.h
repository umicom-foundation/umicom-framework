/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/learning.h
 *
 * PURPOSE:
 *   Define commands that reveal the guided-learning surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_LEARNING_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_LEARNING_H

#include "umicom/studio_runtime/activation.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the command activation rules for interactive learning. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_learning(void);

#ifdef __cplusplus
}
#endif
#endif
