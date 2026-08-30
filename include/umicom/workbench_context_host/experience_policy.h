/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_context_host/experience_policy.h
 *
 * PURPOSE:
 *   Convert Framework-owned application panel metadata into context-link
 *   endpoint behaviour without placing product-specific code in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_POLICY_H
#define UMICOM_WORKBENCH_CONTEXT_HOST_EXPERIENCE_POLICY_H

#include "umicom/application/experience.h"
#include "umicom/workbench_context_host/profile.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiWorkbenchContextHostExperiencePanelPolicy {
    uint32_t structure_size;
    UmiWorkbenchContextHostPanelRole role;
    UmiWorkbenchContextLinkMode mode;
    uint64_t accepted_kinds_mask;
    uint64_t published_kinds_mask;
    bool include_endpoint;
} UmiWorkbenchContextHostExperiencePanelPolicy;

void umi_workbench_context_host_experience_panel_policy_init(
    UmiWorkbenchContextHostExperiencePanelPolicy *policy);

UmiStatus umi_workbench_context_host_experience_panel_policy_resolve(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    UmiWorkbenchContextHostExperiencePanelPolicy *out_policy);

UmiStatus umi_workbench_context_host_experience_panel_policy_validate(
    const UmiWorkbenchContextHostExperiencePanelPolicy *policy);

UmiContextChannelColour umi_workbench_context_host_experience_group_colour(
    const char *context_group_id);

UmiStatus umi_workbench_context_host_experience_group_title(
    const char *context_group_id,
    char *out_title,
    size_t out_title_capacity);

#ifdef __cplusplus
}
#endif

#endif
