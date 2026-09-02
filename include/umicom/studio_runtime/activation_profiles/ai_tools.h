/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/ai_tools.h
 *
 * PURPOSE:
 *   Define command-to-AI Tool Activity activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_AI_TOOLS_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_AI_TOOLS_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile ai tools operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_ai_tools(void);
#ifdef __cplusplus
}
#endif
#endif
