/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/readiness.c
 *
 * PURPOSE:
 *   Aggregate immutable feature state, priority and ownership without duplicating product logic.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/readiness.h"

#include <string.h>

#include "umicom/application/experience_plan.h"

UmiStatus umi_application_readiness_report(
    const UmiApplicationExperienceDefinition *experience,
    UmiApplicationReadinessReport *out_report)
{
    size_t index;
    if (experience == NULL || out_report == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_report, 0, sizeof(*out_report));
    out_report->structure_size = sizeof(*out_report);
    out_report->feature_count = experience->feature_count;
    out_report->readiness_percent = umi_application_experience_readiness_percent(experience);
    out_report->next_feature = umi_application_experience_next_feature(experience);
    for (index = 0U; index < experience->feature_count; ++index) {
        const UmiExperienceFeatureDefinition *feature = &experience->features[index];
        if (feature->state == UMI_EXPERIENCE_FEATURE_PLANNED) out_report->planned_count += 1U;
        else if (feature->state == UMI_EXPERIENCE_FEATURE_FOUNDATION) out_report->foundation_count += 1U;
        else if (feature->state == UMI_EXPERIENCE_FEATURE_IMPLEMENTED) out_report->implemented_count += 1U;
        else if (feature->state == UMI_EXPERIENCE_FEATURE_VERIFIED) out_report->verified_count += 1U;
        if (feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED &&
            feature->priority == UMI_EXPERIENCE_PRIORITY_P0) out_report->p0_open_count += 1U;
        if (feature->state != UMI_EXPERIENCE_FEATURE_VERIFIED &&
            feature->priority == UMI_EXPERIENCE_PRIORITY_P1) out_report->p1_open_count += 1U;
        if (feature->owner == UMI_EXPERIENCE_OWNER_FRAMEWORK) out_report->framework_owned_count += 1U;
        else if (feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION) out_report->application_owned_count += 1U;
        else if (feature->owner == UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER) out_report->external_adapter_count += 1U;
    }
    return UMI_STATUS_OK;
}

int umi_application_readiness_has_open_priority(
    const UmiApplicationReadinessReport *report)
{
    return report != NULL && (report->p0_open_count > 0U || report->p1_open_count > 0U);
}
