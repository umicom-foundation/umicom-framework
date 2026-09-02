/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/ai_tools.h
 *
 * PURPOSE:
 *   Bind the Framework AI Tool Activity IDE surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_TOOLS_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_TOOLS_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio surface profile ai tools operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_ai_tools(void);
#ifdef __cplusplus
}
#endif
#endif
