/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/bookmarks.h
 *
 * PURPOSE:
 *   Define command-to-Bookmarks activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_BOOKMARKS_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_BOOKMARKS_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile bookmark navigation operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *umi_studio_activation_profile_bookmarks(void);
#ifdef __cplusplus
}
#endif
#endif
