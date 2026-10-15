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
#include "../../base/value_construction_internal.h"


#include <string.h>
/*
 * Initialise bootstrap configuration profile from caller-provided values so later
 * operations receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
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
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_configuration_profile_init_build_value(
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

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_configuration_profile_init, UmiBootstrapConfigurationProfile, profile,
    (UmiBootstrapConfigurationProfile *profile, const char *profile_id, const char *parent_profile_id, int32_t priority, bool active),
    umi_bootstrap_configuration_profile_init_build_value(candidate, profile_id, parent_profile_id, priority, active))

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
