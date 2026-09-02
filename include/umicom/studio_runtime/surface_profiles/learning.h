/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/learning.h
 *
 * PURPOSE:
 *   Bind the reusable guided-learning surface into the Application Shell.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_LEARNING_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_LEARNING_H

#include "umicom/studio_runtime/surface_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Return the shared learning surface binding. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_learning(void);

#ifdef __cplusplus
}
#endif
#endif
