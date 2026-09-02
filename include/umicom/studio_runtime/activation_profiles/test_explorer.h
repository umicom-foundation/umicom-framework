/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/test_explorer.h
 *
 * PURPOSE:
 *   Define command-to-Test Explorer activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_TEST_EXPLORER_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_TEST_EXPLORER_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile test explorer operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_test_explorer(void);
#ifdef __cplusplus
}
#endif
#endif
