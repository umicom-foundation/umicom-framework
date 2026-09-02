/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/surface_profiles/live_preview.h
 *
 * PURPOSE:
 *   Publish the Studio binding for the reusable active-document preview.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_SURFACE_LIVE_PREVIEW_H
#define UMICOM_STUDIO_RUNTIME_SURFACE_LIVE_PREVIEW_H

#include "umicom/studio_runtime/surface_binding.h"

/** Return the immutable placement contract for the Live Preview surface. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_live_preview(void);

#endif /* UMICOM_STUDIO_RUNTIME_SURFACE_LIVE_PREVIEW_H */
