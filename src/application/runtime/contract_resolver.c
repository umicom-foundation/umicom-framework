/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/application/runtime/contract_resolver.c
 *
 * PURPOSE:
 *   Resolve application catalogue and experience catalogue entries without duplicating product definitions.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include "umicom/application/runtime/contract_resolver.h"

#include <string.h>

#include "umicom/application/experience_catalogue.h"
#include "umicom/application/portfolio.h"

UmiStatus umi_application_contract_resolve(
    const char *application_id,
    UmiApplicationContract *out_contract)
{
    if (application_id == NULL || out_contract == NULL)
        return UMI_STATUS_INVALID_ARGUMENT;
    memset(out_contract, 0, sizeof(*out_contract));
    out_contract->structure_size = sizeof(*out_contract);
    out_contract->definition = umi_application_portfolio_find(application_id);
    out_contract->experience = umi_application_experience_catalogue_find(application_id);
    return umi_application_contract_validate(out_contract);
}

UmiStatus umi_application_contract_validate(const UmiApplicationContract *contract)
{
    if (contract == NULL || contract->structure_size != sizeof(*contract) ||
        contract->definition == NULL || contract->experience == NULL)
        return UMI_STATUS_NOT_FOUND;
    if (strcmp(contract->definition->application_id,
               contract->experience->application_id) != 0)
        return UMI_STATUS_INVALID_STATE;
    return UMI_STATUS_OK;
}
