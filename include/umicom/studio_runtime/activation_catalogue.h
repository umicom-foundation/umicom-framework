/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/studio_runtime/activation_catalogue.h
 *
 * PURPOSE:
 *   Resolve a successful command into the Studio surface activation rule with
 *   deterministic exact-before-prefix precedence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_STUDIO_RUNTIME_ACTIVATION_CATALOGUE_H
#define UMICOM_STUDIO_RUNTIME_ACTIVATION_CATALOGUE_H

#include "umicom/studio_runtime/activation_profiles/profiles.h"

#ifdef __cplusplus
extern "C" {
#endif

size_t umi_studio_activation_profile_count(void);

const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_at(size_t index);

UmiStatus umi_studio_activation_resolve(
    const char *command_id,
    UmiStudioRuntimeActivationRule *out_rule);

#ifdef __cplusplus
}
#endif
#endif
