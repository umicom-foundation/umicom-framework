/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/live_preview.h
 *
 * PURPOSE:
 *   Publish command routing for the active-document Live Preview surface.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_LIVE_PREVIEW_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_LIVE_PREVIEW_H

#include "umicom/studio_runtime/activation.h"

/* This API is implemented by the C23 Framework. Keep its external symbols
 * in the C ABI when a C++ application includes this header directly. */
#ifdef __cplusplus
extern "C" {
#endif

/** Return the immutable command-routing rules for Live Preview. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_live_preview(void);


#ifdef __cplusplus
}
#endif
#endif /* UMICOM_STUDIO_RUNTIME_ACTIVATION_LIVE_PREVIEW_H */
