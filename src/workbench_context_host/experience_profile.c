/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_context_host/experience_profile.c
 *
 * PURPOSE:
 *   Project canonical application panels and colour groups into a reusable
 *   context-host profile.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_context_host/experience_profile.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"

/* Provide the join identifier operation used by this module and its client applications. */
static UmiStatus join_identifier(
    char *destination,
    size_t capacity,
    const char *left,
    const char *right)
{
    size_t left_length;
    size_t right_length;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (destination == NULL || capacity == 0U || left == NULL ||
        right == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    left_length = strlen(left);
    right_length = strlen(right);
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (left_length + 1U + right_length + 1U > capacity) {
        destination[0] = '\0';
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    memcpy(destination, left, left_length);
    destination[left_length] = '.';
    memcpy(destination + left_length + 1U, right, right_length);
    destination[left_length + 1U + right_length] = '\0';
    return UMI_STATUS_OK;
}

/* Provide the find group operation used by this module and its client applications. */
static UmiWorkbenchContextHostGroupDefinition *find_group(
    UmiWorkbenchContextHostProfile *profile,
    const char *group_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < profile->group_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(profile->groups[index].group_id, group_id) == 0) {
            return &profile->groups[index];
        }
    }
    return NULL;
}

/* Provide the ensure group operation used by this module and its client applications. */
static UmiStatus ensure_group(
    UmiWorkbenchContextHostProfile *profile,
    const char *group_id,
    const UmiWorkbenchContextHostExperiencePanelPolicy *policy)
{
    UmiWorkbenchContextHostGroupDefinition *existing;
    UmiWorkbenchContextHostGroupDefinition definition;
    UmiStatus status;

    existing = find_group(profile, group_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (existing != NULL) {
        existing->allowed_kinds_mask |= policy->accepted_kinds_mask;
        /* Apply this branch only when its contract condition is satisfied. */
        if (policy->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL) {
            existing->default_mode =
                UMI_WORKBENCH_CONTEXT_LINK_MODE_BIDIRECTIONAL;
        }
        ++existing->revision;
        ++profile->revision;
        return UMI_STATUS_OK;
    }

    umi_workbench_context_host_group_definition_init(&definition, group_id);
    status = umi_workbench_context_host_experience_group_title(
        group_id, definition.title, sizeof(definition.title));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    definition.colour =
        umi_workbench_context_host_experience_group_colour(group_id);
    definition.allowed_kinds_mask = policy->accepted_kinds_mask;
    definition.default_mode = policy->mode;
    definition.default_active = profile->group_count == 0U;
    return umi_workbench_context_host_profile_add_group(profile, &definition);
}

/*
 * Provide the add panel endpoint operation used by this module and its client
 * applications.
 */
static UmiStatus add_panel_endpoint(
    const UmiApplicationExperienceDefinition *experience,
    const UmiExperiencePanelDefinition *panel,
    const char *group_id,
    const UmiWorkbenchContextHostExperiencePanelPolicy *policy,
    UmiWorkbenchContextHostProfile *profile)
{
    UmiWorkbenchContextHostEndpoint endpoint;
    char endpoint_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    UmiStatus status;

    status = join_identifier(
        endpoint_id, sizeof(endpoint_id), experience->application_id,
        panel->panel_id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_context_host_endpoint_init(&endpoint, endpoint_id);
    status = umi_workbench_context_host_endpoint_set_identity(
        &endpoint, panel->panel_id, experience->application_id, panel->title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    status = umi_workbench_context_host_endpoint_set_group(
        &endpoint, group_id, policy->mode);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    endpoint.role = policy->role;
    endpoint.state =
        policy->mode == UMI_WORKBENCH_CONTEXT_LINK_MODE_FOLLOW
            ? UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_READY
            : UMI_WORKBENCH_CONTEXT_HOST_ENDPOINT_ACTIVE;
    endpoint.accepted_kinds_mask = policy->accepted_kinds_mask;
    endpoint.published_kinds_mask = policy->published_kinds_mask;
    endpoint.enabled = true;
    return umi_workbench_context_host_profile_add_endpoint(
        profile, &endpoint);
}

/*
 * Initialise workbench context host experience profile options from caller-provided values
 * so later operations receive a known state.
 */
void umi_workbench_context_host_experience_profile_options_init(
    UmiWorkbenchContextHostExperienceProfileOptions *options)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (options == NULL) return;
    memset(options, 0, sizeof(*options));
    options->structure_size = (uint32_t)sizeof(*options);
    options->include_passive_panels = true;
}

/*
 * Provide the workbench context host experience profile build operation used by this
 * module and its client applications.
 */
UmiStatus umi_workbench_context_host_experience_profile_build(
    const UmiApplicationExperienceDefinition *experience,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostProfile *out_profile)
{
    UmiWorkbenchContextHostExperienceProfileOptions defaults;
    const UmiWorkbenchContextHostExperienceProfileOptions *effective;
    char generated_profile_id[UMI_WORKBENCH_CONTEXT_HOST_ID_CAPACITY];
    size_t panel_index;
    UmiStatus status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    status = umi_application_experience_validate(experience);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    umi_workbench_context_host_experience_profile_options_init(&defaults);
    effective = options != NULL ? options : &defaults;
    /* Apply this branch only when its contract condition is satisfied. */
    if (effective->structure_size != sizeof(*effective)) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (effective->profile_id == NULL) {
        status = join_identifier(
            generated_profile_id, sizeof(generated_profile_id),
            experience->application_id, "experience-context");
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    } /* Use this fallback path when the earlier condition does not apply. */ else {
        status = umi_workbench_context_host_copy_text(
            generated_profile_id, sizeof(generated_profile_id),
            effective->profile_id);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    umi_workbench_context_host_profile_init(
        out_profile, generated_profile_id, experience->application_id);
    status = umi_workbench_context_host_profile_set_title(
        out_profile, experience->display_name);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;

    /* Visit each bounded item once so every record receives the same rule. */
    for (panel_index = 0U; panel_index < experience->panel_count;
         ++panel_index) {
        const UmiExperiencePanelDefinition *panel =
            &experience->panels[panel_index];
        UmiWorkbenchContextHostExperiencePanelPolicy default_policy;
        UmiWorkbenchContextHostExperiencePanelPolicy policy;
        const bool is_context_linked =
            (panel->flags & UMI_EXPERIENCE_PANEL_CONTEXT_LINKED) != 0U;

        status = umi_workbench_context_host_experience_panel_policy_resolve(
            experience, panel, &default_policy);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        policy = default_policy;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (effective->policy_resolver != NULL) {
            status = effective->policy_resolver(
                experience, panel, &default_policy, effective->user_data,
                &policy);
            /* Preserve the original failure result so the caller can respond to the correct cause. */
            if (status != UMI_STATUS_OK) return status;
        }
        status = umi_workbench_context_host_experience_panel_policy_validate(
            &policy);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        /* Apply this branch only when its contract condition is satisfied. */
        if (!policy.include_endpoint ||
            (!effective->include_passive_panels && !is_context_linked)) {
            continue;
        }

        status = ensure_group(
            out_profile, panel->context_group_id, &policy);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        status = add_panel_endpoint(
            experience, panel, panel->context_group_id, &policy,
            out_profile);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }

    return umi_workbench_context_host_profile_validate(out_profile);
}

/*
 * Provide the workbench context host experience profile build for application operation
 * used by this module and its client applications.
 */
UmiStatus umi_workbench_context_host_experience_profile_build_for_application(
    const char *application_id,
    const UmiWorkbenchContextHostExperienceProfileOptions *options,
    UmiWorkbenchContextHostProfile *out_profile)
{
    const UmiApplicationExperienceDefinition *experience;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL || out_profile == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    experience = umi_application_experience_catalogue_find(application_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (experience == NULL) return UMI_STATUS_NOT_FOUND;
    return umi_workbench_context_host_experience_profile_build(
        experience, options, out_profile);
}
