/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/design_preview.h
 * PURPOSE: Publish command activation rules for live design preview.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_PREVIEW_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_DESIGN_PREVIEW_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the command rules that reveal the Preview design surface. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_design_preview(void);
#ifdef __cplusplus
}
#endif
#endif
