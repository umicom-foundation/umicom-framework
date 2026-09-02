/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/frontend/conformance/viewport_profile.h
 *
 * PURPOSE:
 *   viewport aspect, pixel density and input-mode classification for frontend adaptation.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_FRONTEND_CONFORMANCE_VIEWPORT_PROFILE_H
#define UMICOM_FRONTEND_CONFORMANCE_VIEWPORT_PROFILE_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "umicom/frontend/conformance/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * List the named fc input mode values accepted by this public contract.
 */
typedef enum UmiFcInputMode { UMI_FC_INPUT_MOUSE_KEYBOARD=1,UMI_FC_INPUT_TOUCH=2,UMI_FC_INPUT_HYBRID=3 } UmiFcInputMode;
/**
 * Represent the fc viewport profile data shared with callers of this public contract.
 */
typedef struct UmiFcViewportProfile { int32_t width; int32_t height; double scale; UmiFcInputMode input; } UmiFcViewportProfile;
/**
 * Provide the fc viewport profile aspect operation used by this module and its client
 * applications.
 */
double umi_fc_viewport_profile_aspect(const UmiFcViewportProfile *profile);
/**
 * Provide the fc viewport profile is portrait operation used by this module and its client
 * applications.
 */
bool umi_fc_viewport_profile_is_portrait(const UmiFcViewportProfile *profile);

#ifdef __cplusplus
}
#endif
#endif
