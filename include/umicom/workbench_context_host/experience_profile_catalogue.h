/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/experience_profile_catalogue.h
 *
 * PURPOSE:
 *   Register context-linked workbench profiles for the complete Umicom
 *   application experience catalogue.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_PROFILE_CATALOGUE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_PROFILE_CATALOGUE_H

#include "umicom/workbench_context_host/experience_profile.h"
#include "umicom/workbench_context_host/profile_catalogue.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the workbench context host experience profile summary data shared with callers
 * of this public contract.
 */
typedef struct UmiWorkbenchContextHostExperienceProfileSummary {
    size_t application_count;
    size_t group_count;
    size_t endpoint_count;
    size_t active_endpoint_count;
    size_t passive_endpoint_count;
} UmiWorkbenchContextHostExperienceProfileSummary;

/**
 * Add workbench context host experience profiles only after its inputs and available
 * capacity have been checked.
 */
UmiStatus umi_workbench_context_host_experience_profiles_register(
    UmiWorkbenchContextHostProfileCatalogue *catalogue,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostExperienceProfileSummary *out_summary);

#ifdef __cplusplus
}
#endif

#endif
