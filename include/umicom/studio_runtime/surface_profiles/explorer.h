/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/explorer.h
 *
 * PURPOSE:
 *   Bind the Framework Explorer IDE surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_EXPLORER_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_EXPLORER_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio surface profile explorer operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_explorer(void);
#ifdef __cplusplus
}
#endif
#endif
