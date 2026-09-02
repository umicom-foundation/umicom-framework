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

/**
 * Return the number of records represented by studio activation profile without changing
 * their state.
 */
size_t umi_studio_activation_profile_count(void);

/**
 * Find studio activation profile while leaving the underlying catalogue or model owned by
 * this module.
 */
const UmiStudioRuntimeActivationProfile *
umi_studio_activation_profile_at(size_t index);

/**
 * Provide the studio activation resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_studio_activation_resolve(
    const char *command_id,
    UmiStudioRuntimeActivationRule *out_rule);

#ifdef __cplusplus
}
#endif
#endif
