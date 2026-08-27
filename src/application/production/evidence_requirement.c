/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/evidence_requirement.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/evidence_requirement.h"

#include <stdio.h>
#include <string.h>

static UmiStatus add_requirement(
    UmiApplicationProductionEvidenceRequirements *requirements,
    UmiApplicationProductionEvidenceKind kind, int required,
    const char *application_id, const char *category, const char *asset_id)
{
    UmiApplicationProductionEvidenceRequirement *entry;
    int written;
    if (requirements->count >= UMI_APPLICATION_PRODUCTION_MAX_EVIDENCE)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry = &requirements->entries[requirements->count];
    written = asset_id == NULL
        ? snprintf(entry->evidence_id, sizeof(entry->evidence_id),
                   "%s:%s", application_id, category)
        : snprintf(entry->evidence_id, sizeof(entry->evidence_id),
                   "%s:%s:%s", application_id, category, asset_id);
    if (written < 0 || (size_t)written >= sizeof(entry->evidence_id))
        return UMI_STATUS_CAPACITY_EXCEEDED;
    entry->kind = kind;
    entry->required = required != 0;
    requirements->required_count += (size_t)entry->required;
    requirements->count += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_production_evidence_requirements_build(
    const UmiApplicationProductionBinding *binding,
    UmiApplicationProductionEvidenceRequirements *out_requirements)
{
    size_t index;
    UmiStatus status;
    if (out_requirements == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_application_production_binding_validate(binding);
    if (status != UMI_STATUS_OK) return status;
    (void)memset(out_requirements, 0, sizeof(*out_requirements));
    status = add_requirement(out_requirements,
        UMI_APPLICATION_PRODUCTION_EVIDENCE_MANIFEST, 1,
        binding->experience->application_id, "manifest", NULL);
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < binding->experience->layout_count; ++index) {
        status = add_requirement(out_requirements,
            UMI_APPLICATION_PRODUCTION_EVIDENCE_LAYOUT, 1,
            binding->experience->application_id, "layout",
            binding->experience->layouts[index].layout_id);
        if (status != UMI_STATUS_OK) return status;
    }
    for (index = 0U; index < binding->experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &binding->experience->features[index];
        status = add_requirement(out_requirements,
            UMI_APPLICATION_PRODUCTION_EVIDENCE_CAPABILITY,
            feature->priority == UMI_EXPERIENCE_PRIORITY_P0 ||
            feature->priority == UMI_EXPERIENCE_PRIORITY_P1,
            binding->experience->application_id, "feature",
            feature->feature_id);
        if (status != UMI_STATUS_OK) return status;
    }
    status = add_requirement(out_requirements,
        UMI_APPLICATION_PRODUCTION_EVIDENCE_TEST, 1,
        binding->experience->application_id, "tests", NULL);
    if (status != UMI_STATUS_OK) return status;
    return add_requirement(out_requirements,
        UMI_APPLICATION_PRODUCTION_EVIDENCE_ACCEPTANCE, 1,
        binding->experience->application_id, "acceptance", NULL);
}

const UmiApplicationProductionEvidenceRequirement *
umi_application_production_evidence_requirements_find(
    const UmiApplicationProductionEvidenceRequirements *requirements,
    const char *evidence_id)
{
    size_t index;
    if (requirements == NULL || evidence_id == NULL) return NULL;
    for (index = 0U; index < requirements->count; ++index)
        if (strcmp(requirements->entries[index].evidence_id,
                   evidence_id) == 0)
            return &requirements->entries[index];
    return NULL;
}

