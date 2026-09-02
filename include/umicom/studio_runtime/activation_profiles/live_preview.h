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

/** Return the immutable command-routing rules for Live Preview. */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_live_preview(void);

#endif /* UMICOM_STUDIO_RUNTIME_ACTIVATION_LIVE_PREVIEW_H */
