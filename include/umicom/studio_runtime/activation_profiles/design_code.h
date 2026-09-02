/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/design_code.h
 * PURPOSE: Publish command activation rules for editable design source.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_CODE_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_CODE_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the command rules that reveal the Code design surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_code(void);
#ifdef __cplusplus
}
#endif
#endif
