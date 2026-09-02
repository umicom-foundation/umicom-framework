/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/component_palette.h
 * PURPOSE: Publish the searchable component palette Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_COMPONENT_PALETTE_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_COMPONENT_PALETTE_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the searchable component list. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_component_palette(void);
#ifdef __cplusplus
}
#endif
#endif
