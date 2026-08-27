/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/capability_requirement.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/capability_requirement.h"

#include <string.h>

static UmiApplicationProductionCapabilityRequirement *find_mutable(
    UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id)
{
    size_t index;
    for (index = 0U; index < requirements->count; ++index)
        if (strcmp(requirements->entries[index].capability_id,
                   capability_id) == 0)
            return &requirements->entries[index];
    return NULL;
}

static UmiStatus require_capability(
    UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id, int panel, int critical)
{
    UmiApplicationProductionCapabilityRequirement *entry;
    size_t length;
    if (capability_id == NULL || capability_id[0] == '\0')
        return UMI_STATUS_OK;
    entry = find_mutable(requirements, capability_id);
    if (entry == NULL) {
        if (requirements->count >=
            UMI_APPLICATION_PRODUCTION_MAX_CAPABILITIES)
            return UMI_STATUS_CAPACITY_EXCEEDED;
        entry = &requirements->entries[requirements->count];
        length = strlen(capability_id);
        if (length >= sizeof(entry->capability_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(entry->capability_id, capability_id, length + 1U);
        requirements->count += 1U;
    }
    if (panel) entry->panel_consumers += 1U;
    else entry->feature_consumers += 1U;
    if (critical) {
        if (entry->critical_consumers == 0U)
            requirements->critical_count += 1U;
        entry->critical_consumers += 1U;
    }
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_capability_requirements_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionCapabilityRequirements *out_requirements)
{
    size_t index;
    UmiStatus status;
    if (out_requirements == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_requirements, 0, sizeof(*out_requirements));
    for (index = 0U; index < binding->experience->panel_count; ++index) {
        const UmiExperiencePanelDefinition *panel =
            &binding->experience->panels[index];
        status = require_capability(
            out_requirements, panel->required_capability, 1,
            (panel->flags & UMI_EXPERIENCE_PANEL_CRITICAL) != 0U);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < binding->experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &binding->experience->features[index];
        status = require_capability(
            out_requirements, feature->required_capability, 0,
            feature->priority == UMI_EXPERIENCE_PRIORITY_P0);
        if (status != UMI_STATUS_OK) return status;
    }
    return UMI_STATUS_OK;
}

const UmiApplicationProductionCapabilityRequirement *
umi_application_production_capability_requirements_find(
    const UmiApplicationProductionCapabilityRequirements *requirements,
    const char *capability_id)
{
    size_t index;
    if (requirements == NULL || capability_id == NULL) return NULL;
    for (index = 0U; index < requirements->count; ++index)
        if (strcmp(requirements->entries[index].capability_id,
                   capability_id) == 0)
            return &requirements->entries[index];
    return NULL;
}

