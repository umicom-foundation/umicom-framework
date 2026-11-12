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

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/** Return the immutable placement contract for the Live Preview surface. */
const UmiStudioRuntimeSurfaceBinding *
umi_studio_surface_profile_live_preview(void);


#ifdef __cplusplus
}
#endif
#endif /* UMICOM_STUDIO_RUNTIME_SURFACE_LIVE_PREVIEW_H */
