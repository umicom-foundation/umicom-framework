/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/runtime_catalogue.c
 *
 * PURPOSE:
 *   Cross-check canonical application and experience catalogues and calculate aggregate readiness.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/runtime_catalogue.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"

/*
 * Provide the application runtime catalogue status operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_runtime_catalogue_status(
    UmiApplicationRuntimeCatalogueStatus *out_status)
{
    size_t index;
    unsigned readiness_total = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_status == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_status, 0, sizeof(*out_status));
    out_status->structure_size = sizeof(*out_status);
    out_status->portfolio_count = umi_application_portfolio_count();
    out_status->experience_count = umi_application_experience_catalogue_count();
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < out_status->portfolio_count; ++index) {
        const UmiApplicationDefinition *definition = umi_application_portfolio_at(index);
        const UmiApplicationExperienceDefinition *experience = definition != NULL
            ? umi_application_experience_catalogue_find(definition->application_id) : NULL;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (experience == NULL) out_status->missing_experience_count += 1U;
        /* Use this fallback path when the earlier condition does not apply. */
        else {
            out_status->aligned_count += 1U;
            readiness_total += umi_application_experience_readiness_percent(experience);
        }
    }
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (out_status->aligned_count > 0U)
        out_status->average_readiness_percent = readiness_total / (unsigned)out_status->aligned_count;
    return UMI_STATUS_OK;
}

/*
 * Check that application runtime catalogue satisfies its contract before another service
 * relies on it.
 */
UmiStatus umi_application_runtime_catalogue_validate(void)
{
    UmiApplicationRuntimeCatalogueStatus status;
    UmiStatus result = umi_application_runtime_catalogue_status(&status);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (result != UMI_STATUS_OK) return result;
    return status.missing_experience_count == 0U ? UMI_STATUS_OK : UMI_STATUS_NOT_FOUND;
}
