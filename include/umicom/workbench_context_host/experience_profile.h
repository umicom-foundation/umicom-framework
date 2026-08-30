/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/experience_profile.h
 *
 * PURPOSE:
 *   Build a complete context-linked workbench profile from a canonical
 *   application experience definition.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_PROFILE_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_PROFILE_H

#include "umicom/workbench_context_host/experience_policy.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef UmiStatus (*UmiWorkbenchContextHostExperiencePolicyResolver)(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    const UmiWorkbenchContextHostExperiencePanelPolicy *default_policy,
    void *user_data,
    UmiWorkbenchContextHostExperiencePanelPolicy *out_policy);

typedef struct UmiWorkbenchContextHostExperienceProfileOptions {
    uint32_t structure_size;
    const char *profile_id;
    UmiWorkbenchContextHostExperiencePolicyResolver policy_resolver;
    void *user_data;
    bool include_passive_panels;
} UmiWorkbenchContextHostExperienceProfileOptions;

void umi_workbench_context_host_experience_profile_options_init(
    UmiWorkbenchContextHostExperienceProfileOptions *options);

UmiStatus umi_workbench_context_host_experience_profile_build(
    const UmiApplicationExperienceDefinition *experience,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostProfile *out_profile);

UmiStatus umi_workbench_context_host_experience_profile_build_for_application(
    const char *application_id,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostProfile *out_profile);

#ifdef __cplusplus
}
#endif

#endif
