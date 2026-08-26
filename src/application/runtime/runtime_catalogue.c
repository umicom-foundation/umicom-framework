/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/runtime_catalogue.c
 *
 * PURPOSE:
 *   Cross-check canonical application and experience catalogues and calculate aggregate readiness.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/runtime_catalogue.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"

UmiStatus umi_application_runtime_catalogue_status(
    UmiApplicationRuntimeCatalogueStatus *out_status)
{
    size_t index;
    unsigned readiness_total = 0U;
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof(*out_status));
    out_status->structure_size = sizeof(*out_status);
    out_status->portfolio_count = umi_application_portfolio_count();
    out_status->experience_count = umi_application_experience_catalogue_count();
    for (index = 0U; index < out_status->portfolio_count; ++index) {
        const UmiApplicationDefinition *definition = umi_application_portfolio_at(index);
        const UmiApplicationExperienceDefinition *experience = definition != NULL
            ? umi_application_experience_catalogue_find(definition->application_id) : NULL;
        if (experience == NULL) out_status->missing_experience_count += 1U;
        else {
            out_status->aligned_count += 1U;
            readiness_total += umi_application_experience_readiness_percent(experience);
        }
    }
    if (out_status->aligned_count > 0U)
        out_status->average_readiness_percent = readiness_total / (unsigned)out_status->aligned_count;
    return UMI_STATUS_OK;
}

UmiStatus umi_application_runtime_catalogue_validate(void)
{
    UmiApplicationRuntimeCatalogueStatus status;
    UmiStatus result = umi_application_runtime_catalogue_status(&status);
    if (result != UMI_STATUS_OK) return result;
    return status.missing_experience_count == 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
