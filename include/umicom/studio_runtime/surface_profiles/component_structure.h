/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/component_structure.h
 * PURPOSE: Publish the component hierarchy Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_COMPONENT_STRUCTURE_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_COMPONENT_STRUCTURE_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the semantic component tree. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_component_structure(void);
#ifdef __cplusplus
}
#endif
#endif
