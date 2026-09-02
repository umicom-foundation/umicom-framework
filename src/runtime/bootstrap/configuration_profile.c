/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_profile.c
 *
 * PURPOSE:
 *   Implement the configuration profile behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_profile.c
 *
 * PURPOSE:
 *   Represent named runtime profiles and parent-profile relationships.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_profile.h"


#include <string.h>
/*
 * Initialise bootstrap configuration profile from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_bootstrap_configuration_profile_init(
    UmiBootstrapConfigurationProfile *profile,
    const char *profile_id,
    const char *parent_profile_id,
    int32_t priority,
    bool active) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || !umi_bootstrap_id_valid(profile_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(profile, 0, sizeof(*profile));
    status = umi_bootstrap_copy_text(profile->profile_id, sizeof(profile->profile_id), profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (parent_profile_id != NULL && parent_profile_id[0] != '\0') {
        /* Use the stable identifier comparison to choose the matching record or policy. */
        if (!umi_bootstrap_id_valid(parent_profile_id) ||
            strcmp(parent_profile_id, profile_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
        status = umi_bootstrap_copy_text(profile->parent_profile_id,
            sizeof(profile->parent_profile_id), parent_profile_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    profile->priority = priority;
    profile->active = active;
    return UMI_STATUS_OK;
}
/*
 * Provide the bootstrap configuration profile matches operation used by this module and
 * its client applications.
 */
bool umi_bootstrap_configuration_profile_matches(
    const UmiBootstrapConfigurationProfile *profile,
    const char *active_profile_id) {
    return profile != NULL && profile->active && active_profile_id != NULL &&
           strcmp(profile->profile_id, active_profile_id) == 0;
}
