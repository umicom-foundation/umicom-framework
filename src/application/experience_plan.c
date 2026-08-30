/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_plan.c
 *
 * PURPOSE:
 *   Resolve deterministic implementation priorities without embedding product code in applications.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_plan.h"

size_t umi_application_experience_priority_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperiencePriority priority)
{
    size_t index;
    size_t count = 0U;
    if (definition == NULL ||
        priority < UMI_EXPERIENCE_PRIORITY_P0 ||
        priority > UMI_EXPERIENCE_PRIORITY_P4)
        return 0U;
    for (index = 0U; index < definition->feature_count; ++index) {
        if (definition->features[index].priority == priority &&
            definition->features[index].state !=
                UMI_EXPERIENCE_FEATURE_VERIFIED)
            ++count;
    }
    return count;
}

const UmiExperienceFeatureDefinition *umi_application_experience_next_feature(
    const UmiApplicationExperienceDefinition *definition)
{
    UmiExperiencePriority priority;
    size_t index;
    if (definition == NULL) return NULL;

    for (priority = UMI_EXPERIENCE_PRIORITY_P0;
         priority <= UMI_EXPERIENCE_PRIORITY_P4;
         priority = (UmiExperiencePriority)((int)priority + 1)) {
        for (index = 0U; index < definition->feature_count; ++index) {
            const UmiExperienceFeatureDefinition *feature =
                &definition->features[index];
            if (feature->priority == priority &&
                feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED)
                return feature;
        }
    }
    return NULL;
}

int umi_application_experience_has_unverified_critical_feature(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    if (definition == NULL) return 0;
    for (index = 0U; index < definition->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &definition->features[index];
        if (feature->priority == UMI_EXPERIENCE_PRIORITY_P0 &&
            feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED)
            return 1;
    }
    return 0;
}
