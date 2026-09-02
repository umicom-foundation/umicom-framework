/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/capability_requirement.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/capability_requirement.h"

#include <string.h>

/* Provide the find mutable operation used by this module and its client applications. */
static UmiApplicationProductionCapabilityRequirement *find_mutable(
    UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id)
{
    size_t index;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < requirements->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(requirements->entries[index].capability_id,
                   capability_id) == 0)
            return &requirements->entries[index];
    return NULL;
}

/*
 * Provide the require capability operation used by this module and its client
 * applications.
 */
static UmiStatus require_capability(
    UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id, int panel, int critical)
{
    UmiApplicationProductionCapabilityRequirement *entry;
    size_t length;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (capability_id == NULL || capability_id[0] == '\0')
        return UMI_STATUS_OK;
    entry = find_mutable(requirements, capability_id);
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (entry == NULL) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (requirements->count >=
            UMI_APPLICATION_PRODUCTION_MAX_CAPABILITIES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        entry = &requirements->entries[requirements->count];
        length = strlen(capability_id);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (length >= sizeof(entry->capability_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(entry->capability_id, capability_id, length + 1U);
        requirements->count += 1U;
    }
    /* Apply this branch only when its contract condition is satisfied. */
    if (panel) entry->panel_consumers += 1U;
    /* Use this fallback path when the earlier condition does not apply. */
    else entry->feature_consumers += 1U;
    /* Apply this branch only when its contract condition is satisfied. */
    if (critical) {
        /* Apply this branch only when its contract condition is satisfied. */
        if (entry->critical_consumers == 0U)
            requirements->critical_count += 1U;
        entry->critical_consumers += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Provide the application production capability requirements build operation used by this
 * module and its client applications.
 */
UmiStatus umi_application_production_capability_requirements_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCapabilityRequirements *out_requirements)
{
    size_t index;
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_requirements == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_requirements, 0, sizeof(*out_requirements));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < binding->experience->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            &binding->experience->panels[index];
        status = require_capability(
            out_requirements, panel->required_capability, 1,
            (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < binding->experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &binding->experience->features[index];
        status = require_capability(
            out_requirements, feature->required_capability, 0,
            feature->priority == UMI_EXPERIENCE_PRIORITY_P0);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

/*
 * Find application production capability requirements while leaving the underlying
 * catalogue or model owned by this module.
 */
const UmiApplicationProductionCapabilityRequirement *
umi_application_production_capability_requirements_find(
    const UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (requirements == NULL || capability_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < requirements->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(requirements->entries[index].capability_id,
                   capability_id) == 0)
            return &requirements->entries[index];
    return NULL;
}

