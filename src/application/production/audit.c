/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/audit.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/audit.h"

#include <string.h>

UmiStatus umi_application_production_gap_audit_build(
    const UmiApplicationProductionPortfolio *portfolio,
    UmiApplicationProductionGapAudit *out_audit)
{
    size_t application_index;
    if (portfolio == NULL || out_audit == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(out_audit, 0, sizeof(*out_audit));
    out_audit->application_count = portfolio->count;
    for (application_index = 0U;
         application_index < portfolio->count; ++application_index) {
        const UmiApplicationProductionRuntime *runtime =
            &portfolio->entries[application_index].runtime;
        size_t feature_index;
        out_audit->panel_count += runtime->panels.count;
        out_audit->layout_count += runtime->layouts.count;
        out_audit->feature_count += runtime->features.count;
        out_audit->uncovered_panel_count +=
            runtime->panels.uncovered_count;
        out_audit->manifest_drift_count +=
            (size_t)!runtime->manifest_drift.compatible;
        for (feature_index = 0U;
             feature_index < runtime->features.count; ++feature_index) {
            const UmiExperienceFeatureDefinition *feature =
                runtime->features.entries[feature_index].feature;
            out_audit->framework_feature_count +=
                (size_t)(feature->owner == UMI_EXPERIENCE_OWNER_FRAMEWORK);
            out_audit->application_feature_count +=
                (size_t)(feature->owner == UMI_EXPERIENCE_OWNER_APPLICATION);
            out_audit->external_adapter_feature_count +=
                (size_t)(feature->owner ==
                         UMI_EXPERIENCE_OWNER_EXTERNAL_ADAPTER);
            out_audit->planned_feature_count +=
                (size_t)(feature->state == UMI_EXPERIENCE_FEATURE_PLANNED);
            out_audit->foundation_feature_count +=
                (size_t)(feature->state ==
                         UMI_EXPERIENCE_FEATURE_FOUNDATION);
            out_audit->implemented_feature_count +=
                (size_t)(feature->state ==
                         UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
            out_audit->verified_feature_count +=
                (size_t)(feature->state ==
                         UMI_EXPERIENCE_FEATURE_VERIFIED);
        }
    }
    return UMI_STATUS_OK;
}

