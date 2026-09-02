/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/experience_profile_catalogue.c
 *
 * PURPOSE:
 *   Build and register context-host profiles for every canonical Umicom
 *   application experience.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/experience_profile_catalogue.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

/*
 * Add workbench context host experience profiles only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_workbench_context_host_experience_profiles_register(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostExperienceProfileSummary *out_summary)
{
    UmiWorkbenchContextHostExperienceProfileSummary summary;
    const size_t application_count =
        umi_application_experience_catalogue_count();
    size_t application_index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (catalogue == NULL || application_count >
            UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options != NULL && options->profile_id != NULL) {
        /* A shared explicit identifier would collide for every application. */
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(&summary, 0, sizeof(summary));

    /* Visit each bounded item once so every record receives the same rule. */
    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(application_index);
        UmiWorkbenchContextHostProfile profile;
        size_t endpoint_index;
        UmiStatus status;

        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (experience == NULL) return UMI_STATUS_INVALID_STATE;
        status = umi_workbench_context_host_experience_profile_build(
            experience, options, &profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = umi_workbench_context_host_profile_catalogue_upsert(
            catalogue, &profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;

        ++summary.application_count;
        summary.group_count += profile.group_count;
        summary.endpoint_count += profile.endpoint_count;
        /* Visit each bounded item once so every record receives the same rule. */
        for (endpoint_index = 0U;
             endpoint_index < profile.endpoint_count;
             ++endpoint_index) {
            /* Apply this branch only when its contract condition is satisfied. */
            if (profile.endpoints[endpoint_index].published_kinds_mask != 0U) {
                ++summary.active_endpoint_count;
            } /* Use this fallback path when the earlier condition does not apply. */ else {
                ++summary.passive_endpoint_count;
            }
        }
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_summary != NULL) *out_summary = summary;
    return UMI_STATUS_OK;
}
