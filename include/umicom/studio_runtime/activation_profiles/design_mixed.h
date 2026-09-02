/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/design_mixed.h
 * PURPOSE: Publish command activation rules for split design and source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_MIXED_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_MIXED_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the command rules that reveal the Mixed design surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_mixed(void);
#ifdef __cplusplus
}
#endif
#endif
