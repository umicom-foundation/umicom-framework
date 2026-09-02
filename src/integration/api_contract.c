/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/api_contract.c
 *
 * PURPOSE:
 *   Implement the api contract behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | API contracts | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/api_contract.h"
#include <string.h>

/*
 * Initialise integration api contract from caller-provided values so later operations
 * receive a known state.
 */
UmiStatus umi_integration_api_contract_init(UmiIntegrationApiContract *contract,const char *id,const char *title,const char *version,const char *profile_id)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    (void)memset(contract,0,sizeof(*contract));
    status = umi_integration_designer_copy(contract->id,sizeof(contract->id),id);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->title,sizeof(contract->title),title);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->version,sizeof(contract->version),version);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) status = umi_integration_designer_copy(contract->profile_id,sizeof(contract->profile_id),profile_id);
    return status;
}

/*
 * Provide the integration api contract find operation operation used by this module and
 * its client applications.
 */
const UmiIntegrationApiOperation *umi_integration_api_contract_find_operation(const UmiIntegrationApiContract *contract,const char *operation_id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL || operation_id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < contract->operation_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(contract->operations[index].id,operation_id) == 0) return &contract->operations[index];
    return NULL;
}

/*
 * Provide the integration api contract add operation operation used by this module and its
 * client applications.
 */
UmiStatus umi_integration_api_contract_add_operation(UmiIntegrationApiContract *contract,const UmiIntegrationApiOperation *operation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL || operation == NULL || operation->id[0] == '\0' || operation->path[0] == '\0') return UMI_STATUS_INVALID_ARGUMENT;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_api_contract_find_operation(contract,operation->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (contract->operation_count >= UMI_INTEGRATION_DESIGNER_MAX_OPERATIONS) return UMI_STATUS_CAPACITY_EXCEEDED;
    contract->operations[contract->operation_count] = *operation;
    contract->operation_count += 1U;
    return UMI_STATUS_OK;
}

/*
 * Check that integration api contract satisfies its contract before another service relies
 * on it.
 */
UmiStatus umi_integration_api_contract_validate(const UmiIntegrationApiContract *contract,UmiIntegrationDesignerValidation *validation)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (contract == NULL || validation == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    validation->count = 0U;
    /* Apply this operation only while the related capability or state is available. */
    if (contract->id[0] == '\0') (void)umi_integration_designer_validation_add(validation,"contract.id","Contract id is required");
    /* Use the stable identifier comparison to choose the matching record or policy. */
    if (contract->profile_id[0] == '\0') (void)umi_integration_designer_validation_add(validation,"contract.profile_id","Connection profile is required");
    /* Apply this operation only while the related capability or state is available. */
    if (contract->operation_count == 0U) (void)umi_integration_designer_validation_add(validation,"contract.operations","At least one operation is required");
    return validation->count == 0U ? UMI_STATUS_OK : UMI_STATUS_INVALID_STATE;
}
