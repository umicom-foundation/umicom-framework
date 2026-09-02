/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/documentation.h
 *
 * PURPOSE:
 *   Bind the reusable documentation and HTML preview surface into the
 *   Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_DOCUMENTATION_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_DOCUMENTATION_H

#include "umicom/studio_runtime/surface_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the shared documentation surface binding. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_documentation(void);

#ifdef __cplusplus
}
#endif
#endif
