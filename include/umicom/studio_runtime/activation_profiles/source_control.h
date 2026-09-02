/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/source_control.h
 *
 * PURPOSE:
 *   Define command-to-Source Control activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_SOURCE_CONTROL_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_SOURCE_CONTROL_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile source control operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_source_control(void);
#ifdef __cplusplus
}
#endif
#endif
