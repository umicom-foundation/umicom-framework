/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/design_code.h
 * PURPOSE: Publish the editable design-source Studio surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_CODE_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_DESIGN_CODE_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the design source editor. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_design_code(void);
#ifdef __cplusplus
}
#endif
#endif
