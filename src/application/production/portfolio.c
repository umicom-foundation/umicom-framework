/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/production/portfolio.c
 *
 * PURPOSE:
 *   Implement one bounded part of the Framework-owned application production
 *   control plane while product and frontend code remain independently owned.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/application/production/portfolio.h"

#include <stdio.h>
#include <string.h>

#include "umicom/application/experience_catalogue.h"

/* Provide the module suffix operation used by this module and its client applications. */
static const char *module_suffix(const char *application_id)
{
    const char *cursor = application_id;
    const char *suffix = application_id;
    /*
     * Continue only while work remains available; the loop body advances the state on each
     * pass.
     */
    while (*cursor != '\0') {
        /* Apply this branch only when its contract condition is satisfied. */
        if (*cursor == '.') suffix = cursor + 1;
        cursor += 1;
    }
    return suffix;
}

/*
 * Provide the application production portfolio build operation used by this module and its
 * client applications.
 */
UmiStatus umi_application_production_portfolio_build(
    UmiApplicationCapabilityProbe probe, void *probe_context,
    UmiApplicationProductionPortfolio *out_portfolio)
{
    size_t index;
    size_t count;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_portfolio == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    count = umi_application_experience_catalogue_count();
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (count > UMI_APPLICATION_PRODUCTION_MAX_APPLICATIONS)
        return UMI_STATUS_CAPACITY_EXCEEDED;
    (void)memset(out_portfolio, 0, sizeof(*out_portfolio));
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < count; ++index) {
        const UmiApplicationExperienceDefinition *experience =
            umi_application_experience_catalogue_at(index);
        UmiApplicationProductionPortfolioEntry *entry;
        char module_name[UMI_APPLICATION_PRODUCTION_ID_CAPACITY];
        int written;
        UmiStatus status;
        /*
         * Protect caller-owned memory by checking that required state is available before it is
         * used.
         */
        if (experience == NULL) return UMI_STATUS_INVALID_STATE;
        entry = &out_portfolio->entries[out_portfolio->count];
        written = snprintf(module_name, sizeof(module_name),
                           "%s", module_suffix(experience->application_id));
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(module_name))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        (void)memcpy(entry->module_id, module_name, (size_t)written + 1U);
        written = snprintf(entry->executable_id, sizeof(entry->executable_id),
                           "umicom-%s", module_name);
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (written < 0 || (size_t)written >= sizeof(entry->executable_id))
            return UMI_STATUS_CAPACITY_EXCEEDED;
        entry->adoption.structure_size = sizeof(entry->adoption);
        entry->adoption.module_id = entry->module_id;
        entry->adoption.application_id = experience->application_id;
        entry->adoption.display_name = experience->display_name;
        entry->adoption.executable_id = entry->executable_id;
        entry->adoption.frontend_flags =
            UMI_PRODUCT_FRONTEND_FLAG_CONSOLE |
            UMI_PRODUCT_FRONTEND_FLAG_HEADLESS;
        entry->adoption.manifest_available = 1;
        entry->adoption.composition_available = 1;
        entry->adoption.executable_available = 1;
        entry->adoption.tests_available = 1;
        status = umi_application_production_runtime_init(
            &entry->adoption, probe, probe_context, &entry->runtime);
        /* Preserve the original failure result so the caller can respond to the correct cause. */
        if (status != UMI_STATUS_OK) return status;
        out_portfolio->count += 1U;
    }
    return UMI_STATUS_OK;
}

/*
 * Find application production portfolio while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiApplicationProductionPortfolioEntry *
umi_application_production_portfolio_find(
    const UmiApplicationProductionPortfolio *portfolio,
    const char *application_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (portfolio == NULL || application_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < portfolio->count; ++index)
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(portfolio->entries[index].adoption.application_id,
                   application_id) == 0)
            return &portfolio->entries[index];
    return NULL;
}
