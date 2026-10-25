/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_profiles/kanban.h
 *
 * PURPOSE:
 *   Define command-to-Project Board activation rules.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_KANBAN_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_PROFILE_KANBAN_H
#include "umicom/studio_runtime/activation.h"
#ifdef __cplusplus
extern "C" {
#endif
/**
 * Provide the studio activation profile project board operation used by this module and
 * its client applications.
 */
const UmiStudioRuntimeActivationProfile *umi_studio_activation_profile_kanban(void);
#ifdef __cplusplus
}
#endif
#endif
