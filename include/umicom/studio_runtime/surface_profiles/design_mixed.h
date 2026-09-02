/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/design_mixed.h
 * PURPOSE: Publish the split source-and-design Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_MIXED_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_MIXED_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the mixed authoring surface. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_design_mixed(void);
#ifdef __cplusplus
}
#endif
#endif
