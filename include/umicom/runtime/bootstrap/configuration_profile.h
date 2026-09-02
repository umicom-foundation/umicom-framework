/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_profile.h
 *
 * PURPOSE:
 *   Represent named runtime profiles and parent-profile relationships.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROFILE_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROFILE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Initialise bootstrap configuration profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_bootstrap_configuration_profile_init(
    UmiBootstrapConfigurationProfile *profile,
    const char *profile_id,
    const char *parent_profile_id,
    int32_t priority,
    bool active);
/**
 * Provide the bootstrap configuration profile matches operation used by this module and
 * its client applications.
 */
bool umi_bootstrap_configuration_profile_matches(
    const UmiBootstrapConfigurationProfile *profile,
    const char *active_profile_id);

#ifdef __cplusplus
}
#endif

#endif
