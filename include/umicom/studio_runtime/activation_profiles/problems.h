/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/problems.h
 *
 * PURPOSE:
 *   Define command-to-Problems activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_PROBLEMS_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_PROBLEMS_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile problems operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_problems(void);
#ifdef __cplusplus
}
#endif
#endif
