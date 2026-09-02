/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/source_control.h
 *
 * PURPOSE:
 *   Bind the Framework Source Control IDE surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_SOURCE_CONTROL_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_SOURCE_CONTROL_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio surface profile source control operation used by this module and its
 * client applications.
 */
const UmiStudioRuntimeSurfaceBinding *umi_studio_surface_profile_source_control(void);
#ifdef __cplusplus
}
#endif
#endif
