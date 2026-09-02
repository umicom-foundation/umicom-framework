/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/design_preview.h
 * PURPOSE: Publish the rendered design-preview Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_PREVIEW_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_PREVIEW_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the live preview surface. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_design_preview(void);
#ifdef __cplusplus
}
#endif
#endif
