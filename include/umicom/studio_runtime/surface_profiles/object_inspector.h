/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/object_inspector.h
 * PURPOSE: Publish the selected-component property inspector surface binding.
 * AUTHOR AND ORGANISATION: Sammy Hegab, Umicom Foundation
 * LICENCE: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_OBJECT_INSPECTOR_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_OBJECT_INSPECTOR_H
#include "umicom/studio_runtime/surface_binding.h"
#ifdef __cplusplus
extern "C" {
#endif
/* Return the immutable binding used to place the typed property inspector. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_object_inspector(void);
#ifdef __cplusplus
}
#endif
#endif
