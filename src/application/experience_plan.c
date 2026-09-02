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

/*
 * Return the number of records represented by application experience priority without
 * changing their state.
 */
size_t umi_application_experience_priority_count(
    const UmiApplicationExperienceDefinition *definition,
    UmiExperiencePriority priority)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL ||
        priority < UMI_EXPERIENCE_PRIORITY_P0 ||
        priority > UMI_EXPERIENCE_PRIORITY_P4)
        return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (definition->features[index].priority == priority &&
            definition->features[index].state !=
                UMI_EXPERIENCE_FEATURE_VERIFIED)
            ++count;
    }
    return count;
}

/*
 * Provide the application experience next feature operation used by this module and its
 * client applications.
 */
const UmiExperienceFeatureDefinition *umi_application_experience_next_feature(
    const UmiApplicationExperienceDefinition *definition)
{
    UmiExperiencePriority priority;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return NULL;

    /* Visit each bounded item once so every record receives the same rule. */
    for (priority = UMI_EXPERIENCE_PRIORITY_P0;
         priority <= UMI_EXPERIENCE_PRIORITY_P4;
         priority = (UmiExperiencePriority)((int)priority + 1)) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 0U; index < definition->feature_count; ++index) {
            const UmiExperienceFeatureDefinition *feature =
                &definition->features[index];
            /* Apply this branch only when its contract condition is satisfied. */
            if (feature->priority == priority &&
                feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED)
                return feature;
        }
    }
    return NULL;
}

/*
 * Provide the application experience has unverified critical feature operation used by
 * this module and its client applications.
 */
int umi_application_experience_has_unverified_critical_feature(
    const UmiApplicationExperienceDefinition *definition)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (definition == NULL) return 0;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < definition->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature =
            &definition->features[index];
        /* Apply this branch only when its contract condition is satisfied. */
        if (feature->priority == UMI_EXPERIENCE_PRIORITY_P0 &&
            feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED)
            return 1;
    }
    return 0;
}
