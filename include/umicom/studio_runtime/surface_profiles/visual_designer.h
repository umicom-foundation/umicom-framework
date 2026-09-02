/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/visual_designer.h
 * PURPOSE: Publish the visual designer Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_VISUAL_DESIGNER_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_VISUAL_DESIGNER_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the visual design canvas. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_visual_designer(void);
#ifdef __cplusplus
}
#endif
#endif
