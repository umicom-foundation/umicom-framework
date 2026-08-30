/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application_ui/feature_backlog_view.c
 *
 * PURPOSE:
 *   Filter incomplete canonical features into a stable implementation backlog projection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application_ui/feature_backlog_view.h"

size_t umi_application_ui_feature_backlog_count(
    const UmiApplicationExperienceDefinition *experience)
{
    size_t index;
    size_t count = 0U;
    if (experience == NULL) return 0U;
    for (index = 0U; index < experience->feature_count; ++index)
        if (experience->features[index].state != UMI_EXPERIENCE_FEATURE_VERIFIED) count += 1U;
    return count;
}

UmiStatus umi_application_ui_feature_backlog_row(
    const UmiApplicationExperienceDefinition *experience,
    size_t backlog_index,
    UmiApplicationFeatureProjection *out_row)
{
    size_t index;
    size_t cursor = 0U;
    if (experience == NULL || out_row == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    for (index = 0U; index < experience->feature_count; ++index) {
        if (experience->features[index].state == UMI_EXPERIENCE_FEATURE_VERIFIED) continue;
        if (cursor == backlog_index)
            return umi_application_feature_projection_at(experience, index, out_row);
        cursor += 1U;
    }
    return UMI_STATUS_NOT_FOUND;
}
