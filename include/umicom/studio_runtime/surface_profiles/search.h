/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/search.h
 *
 * PURPOSE:
 *   Bind the Framework Search IDE surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_SEARCH_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_SEARCH_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio surface profile search operation used by this module and its client
 * applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_search(void);
#ifdef __cplusplus
}
#endif
#endif
