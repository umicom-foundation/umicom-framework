/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/ai_chat.h
 *
 * PURPOSE:
 *   Bind the Framework AI Coding Chat IDE surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_CHAT_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_CHAT_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio surface profile ai chat operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_ai_chat(void);
#ifdef __cplusplus
}
#endif
#endif
