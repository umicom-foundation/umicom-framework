/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/contract_resolver.c
 *
 * PURPOSE:
 *   Resolve application catalogue and experience catalogue entries without duplicating product definitions.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/contract_resolver.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"

/*
 * Provide the application contract resolve operation used by this module and its client
 * applications.
 */
UmiStatus umi_application_contract_resolve(
    const char *application_id,
    UmiApplicationContract *out_contract)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (application_id == NULL || out_contract == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_contract, 0, sizeof(*out_contract));
    out_contract->structure_size = sizeof(*out_contract);
    out_contract->definition = umi_application_portfolio_find(application_id);
    out_contract->experience = umi_application_experience_catalogue_find(application_id);
    return umi_application_contract_validate(out_contract);
}

/*
 * Check that application contract satisfies its contract before another service relies on
 * it.
 */
UmiStatus umi_application_contract_validate(const UmiApplicationContract *contract)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL || contract->structure_size != sizeof(*contract) ||
        contract->definition == NULL || contract->experience == NULL)
        return UMI_STATUS_NOT_FOUND;
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (strcmp(contract->definition->application_id,
               contract->experience->application_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
