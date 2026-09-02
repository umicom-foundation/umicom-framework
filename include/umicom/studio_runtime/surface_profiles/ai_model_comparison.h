/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/ai_model_comparison.h
 *
 * PURPOSE:
 *   Bind the reusable AI model comparison surface into a Studio workbench.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_MODEL_COMPARISON_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_PROFILE_AI_MODEL_COMPARISON_H

#include "umicom/studio_runtime/surface_binding.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Provide the studio surface profile ai model comparison operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_ai_model_comparison(void);

#ifdef __cplusplus
}
#endif
#endif
