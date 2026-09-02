/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/ai_approvals.h
 *
 * PURPOSE:
 *   Define command-to-AI Approvals activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_AI_APPROVALS_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_AI_APPROVALS_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile ai approvals operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_approvals(void);
#ifdef __cplusplus
}
#endif
#endif
