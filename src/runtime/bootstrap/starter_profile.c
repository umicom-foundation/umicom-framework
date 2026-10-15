/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/runtime/bootstrap/starter_profile.c
 *
 * PURPOSE:
 *   Implement the starter profile behavior for
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
 * File: src/runtime/bootstrap/starter_profile.c
 *
 * PURPOSE:
 *   Group application starters into reusable environment/application profiles.
 *---------------------------------------------------------------------------*/
#include "umicom/runtime/bootstrap/starter_profile.h"
#include "../../base/value_construction_internal.h"
#include "umicom/runtime/bootstrap/starter_plan.h"


#include <string.h>
/*
 * Initialise bootstrap starter profile from caller-provided values so later operations
 * receive a known state.
 */
/* Construction formerly wrote into the live destination before all inputs
 * were accepted. The staged wrapper below now preserves the previous value on
 * failure and supports inputs borrowed from it. Keep this implementation for
 * review of the original defaults and failure behaviour. */
#if 0
UmiStatus umi_bootstrap_starter_profile_init(UmiBootstrapStarterProfile *profile,
                                             const char *profile_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || !umi_bootstrap_id_valid(profile_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(profile, 0, sizeof(*profile));
    status = umi_bootstrap_copy_text(profile->profile_id, sizeof(profile->profile_id),
                                     profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_bootstrap_starter_plan_init(&profile->starters);
    return UMI_STATUS_OK;
}
#endif

/* This private builder retains the original field initialization and validation.
 * It now receives temporary storage owned by the shared publication wrapper. */
static UmiStatus umi_bootstrap_starter_profile_init_build_value(UmiBootstrapStarterProfile *profile,
                                             const char *profile_id) {
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL || !umi_bootstrap_id_valid(profile_id))
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(profile, 0, sizeof(*profile));
    status = umi_bootstrap_copy_text(profile->profile_id, sizeof(profile->profile_id),
                                     profile_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    umi_bootstrap_starter_plan_init(&profile->starters);
    return UMI_STATUS_OK;
}

UMI_DEFINE_STAGED_VALUE_INIT(umi_bootstrap_starter_profile_init, UmiBootstrapStarterProfile, profile,
    (UmiBootstrapStarterProfile *profile, const char *profile_id),
    umi_bootstrap_starter_profile_init_build_value(candidate, profile_id))

/*
 * Add bootstrap starter profile only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_bootstrap_starter_profile_add(
    UmiBootstrapStarterProfile *profile,
    const UmiBootstrapStarterCatalogue *catalogue,
    const char *starter_id) {
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (profile == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    return umi_bootstrap_starter_plan_add(&profile->starters, catalogue, starter_id);
}
