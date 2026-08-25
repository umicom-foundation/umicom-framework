/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/configuration_profile.c
 *
 * PURPOSE:
 *   Represent named runtime profiles and parent-profile relationships.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/configuration_profile.h"


#include <string.h>
UmiStatus umi_bootstrap_configuration_profile_init(
    UmiBootstrapConfigurationProfile *profile,
    const char *profile_id,
    const char *parent_profile_id,
    int32_t priority,
    bool active) {
    UmiStatus status;
    if (profile == NULL || !umi_bootstrap_id_valid(profile_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(profile, 0, sizeof(*profile));
    status = umi_bootstrap_copy_text(profile->profile_id, sizeof(profile->profile_id), profile_id);
    if (status != UMI_STATUS_OK) return status;
    if (parent_profile_id != NULL && parent_profile_id[0] != '\0') {
        if (!umi_bootstrap_id_valid(parent_profile_id) ||
            strcmp(parent_profile_id, profile_id) == 0) return UMI_STATUS_INVALID_ARGUMENT;
        status = umi_bootstrap_copy_text(profile->parent_profile_id,
            sizeof(profile->parent_profile_id), parent_profile_id);
        if (status != UMI_STATUS_OK) return status;
    }
    profile->priority = priority;
    profile->active = active;
    return UMI_STATUS_OK;
}
bool umi_bootstrap_configuration_profile_matches(
    const UmiBootstrapConfigurationProfile *profile,
    const char *active_profile_id) {
    return profile != NULL && profile->active && active_profile_id != NULL &&
           strcmp(profile->profile_id, active_profile_id) == 0;
}
