/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/documentation.h
 *
 * PURPOSE:
 *   Define commands that reveal the documentation and HTML preview surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_DOCUMENTATION_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_DOCUMENTATION_H

#include "umicom/studio_runtime/activation.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the command activation rules for documentation. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_documentation(void);

#ifdef __cplusplus
}
#endif
#endif
