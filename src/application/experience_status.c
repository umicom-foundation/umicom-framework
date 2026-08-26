/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/experience_status.c
 *
 * PURPOSE:
 *   Build deterministic product readiness snapshots from canonical experience metadata.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/experience_status.h"

#include <string.h>

UmiStatus umi_application_experience_status(
    const UmiApplicationExperienceDefinition *definition,
    UmiApplicationExperienceStatus *out_status)
{
    UmiStatus status;
    if (definition == NULL || out_status == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;

    status = umi_application_experience_validate(definition);
    if (status != UMI_STATUS_OK) return status;

    memset(out_status, 0, sizeof(*out_status));
    out_status->structure_size = sizeof(*out_status);
    out_status->panel_count = definition->panel_count;
    out_status->layout_count = definition->layout_count;
    out_status->feature_count = definition->feature_count;
    out_status->planned_count =
        umi_application_experience_feature_state_count(
            definition, UMI_EXPERIENCE_FEATURE_PLANNED);
    out_status->foundation_count =
        umi_application_experience_feature_state_count(
            definition, UMI_EXPERIENCE_FEATURE_FOUNDATION);
    out_status->implemented_count =
        umi_application_experience_feature_state_count(
            definition, UMI_EXPERIENCE_FEATURE_IMPLEMENTED);
    out_status->verified_count =
        umi_application_experience_feature_state_count(
            definition, UMI_EXPERIENCE_FEATURE_VERIFIED);
    out_status->readiness_percent =
        umi_application_experience_readiness_percent(definition);
    return UMI_STATUS_OK;
}
