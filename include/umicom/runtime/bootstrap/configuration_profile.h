/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/runtime/bootstrap/configuration_profile.h
 *
 * PURPOSE:
 *   Represent named runtime profiles and parent-profile relationships.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROFILE_H
#define UMICOM_RUNTIME_BOOTSTRAP_CONFIGURATION_PROFILE_H

#include "umicom/runtime/bootstrap/types.h"

#ifdef __cplusplus
extern "C" {
#endif


UmiStatus umi_bootstrap_configuration_profile_init(
    UmiBootstrapConfigurationProfile *profile,
    const char *profile_id,
    const char *parent_profile_id,
    int32_t priority,
    bool active);
bool umi_bootstrap_configuration_profile_matches(
    const UmiBootstrapConfigurationProfile *profile,
    const char *active_profile_id);

#ifdef __cplusplus
}
#endif

#endif
