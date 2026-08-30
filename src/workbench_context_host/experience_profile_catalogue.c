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

UmiStatus umi_workbench_context_host_experience_profiles_register(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostExperienceProfileSummary *out_summary)
{
    UmiWorkbenchContextHostExperienceProfileSummary summary;
    const size_t application_count =
        umi_application_experience_catalogue_count();
    size_t application_index;

    if (catalogue == NULL || application_count >
            UMI_WORKBENCH_CONTEXT_HOST_MAX_PROFILES) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (options != NULL && options->profile_id != NULL) {
        /* A shared explicit identifier would collide for every application. */
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    memset(&summary, 0, sizeof(summary));

    for (application_index = 0U; application_index < application_count;
         ++application_index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(application_index);
        UmiWorkbenchContextHostProfile profile;
        size_t endpoint_index;
        UmiStatus status;

        if (experience == NULL) return UMI_STATUS_INVALID_STATE;
        status = umi_workbench_context_host_experience_profile_build(
            experience, options, &profile);
        if (status != UMI_STATUS_OK) return status;
        status = umi_workbench_context_host_profile_catalogue_upsert(
            catalogue, &profile);
        if (status != UMI_STATUS_OK) return status;

        ++summary.application_count;
        summary.group_count += profile.group_count;
        summary.endpoint_count += profile.endpoint_count;
        for (endpoint_index = 0U;
             endpoint_index < profile.endpoint_count;
             ++endpoint_index) {
            if (profile.endpoints[endpoint_index].published_kinds_mask != 0U) {
                ++summary.active_endpoint_count;
            } else {
                ++summary.passive_endpoint_count;
            }
        }
    }

    if (out_summary != NULL) *out_summary = summary;
    return UMI_STATUS_OK;
}
