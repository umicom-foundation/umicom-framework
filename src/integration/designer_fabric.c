/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/integration/designer_fabric.c
 *
 * PURPOSE:
 *   Implement the designer fabric behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Umicom Framework | Integration designer fabric | Sammy Hegab | Umicom Foundation | MIT */
#include "umicom/integration/designer_fabric.h"
#include <string.h>

/*
 * Initialise integration designer fabric from caller-provided values so later operations
 * receive a known state.
 */
void umi_integration_designer_fabric_init(UmiIntegrationDesignerFabric *fabric)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL) return;
    (void)memset(fabric,0,sizeof(*fabric));
    fabric->revision = 1U;
}

/*
 * Provide the integration designer fabric add contract operation used by this module and
 * its client applications.
 */
UmiStatus umi_integration_designer_fabric_add_contract(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationApiContract *contract)
{
    size_t index;
    UmiIntegrationDesignerValidation validation = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL || contract == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_integration_api_contract_validate(contract,&validation) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fabric->contract_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(fabric->contracts[index].id,contract->id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fabric->contract_count >= UMI_INTEGRATION_DESIGNER_MAX_PROFILES) return UMI_STATUS_CAPACITY_EXCEEDED;
    fabric->contracts[fabric->contract_count] = *contract;
    fabric->contract_count += 1U;
    fabric->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration designer fabric add mapping operation used by this module and
 * its client applications.
 */
UmiStatus umi_integration_designer_fabric_add_mapping(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationFieldMapping *mapping)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL || mapping == NULL || mapping->id[0] == '\0' || mapping->rule_count == 0U) return UMI_STATUS_INVALID_ARGUMENT;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fabric->mapping_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(fabric->mappings[index].id,mapping->id) == 0) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fabric->mapping_count >= UMI_INTEGRATION_DESIGNER_MAX_MAPPINGS) return UMI_STATUS_CAPACITY_EXCEEDED;
    fabric->mappings[fabric->mapping_count] = *mapping;
    fabric->mapping_count += 1U;
    fabric->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Provide the integration designer fabric find workflow operation used by this module and
 * its client applications.
 */
const UmiIntegrationWorkflow *umi_integration_designer_fabric_find_workflow(const UmiIntegrationDesignerFabric *fabric,const char *id)
{
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL || id == NULL) return NULL;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < fabric->workflow_count; ++index) /* Keep the operation inside its valid bounds before reading, writing or adding data. */ if (strcmp(fabric->workflows[index].id,id) == 0) return &fabric->workflows[index];
    return NULL;
}

/*
 * Provide the integration designer fabric add workflow operation used by this module and
 * its client applications.
 */
UmiStatus umi_integration_designer_fabric_add_workflow(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationWorkflow *workflow)
{
    UmiIntegrationDesignerValidation validation = {0};
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL || workflow == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (umi_integration_workflow_validate(workflow,&validation) != UMI_STATUS_OK) return UMI_STATUS_INVALID_STATE;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (umi_integration_designer_fabric_find_workflow(fabric,workflow->id) != NULL) return UMI_STATUS_ALREADY_EXISTS;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (fabric->workflow_count >= UMI_INTEGRATION_DESIGNER_MAX_WORKFLOWS) return UMI_STATUS_CAPACITY_EXCEEDED;
    fabric->workflows[fabric->workflow_count] = *workflow;
    fabric->workflow_count += 1U;
    fabric->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Perform integration designer fabric record through the module contract so client
 * applications do not duplicate its policy.
 */
UmiStatus umi_integration_designer_fabric_record_run(UmiIntegrationDesignerFabric *fabric,const UmiIntegrationSimulation *simulation)
{
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_integration_run_history_append(&fabric->runs,simulation);
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status == UMI_STATUS_OK) fabric->revision += 1U;
    return status;
}

/*
 * Provide the integration designer fabric snapshot operation used by this module and its
 * client applications.
 */
void umi_integration_designer_fabric_snapshot(const UmiIntegrationDesignerFabric *fabric,UmiIntegrationDesignerSnapshot *out_snapshot)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_snapshot == NULL) return;
    (void)memset(out_snapshot,0,sizeof(*out_snapshot));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (fabric == NULL) return;
    out_snapshot->connections = fabric->connections.count;
    out_snapshot->schemas = fabric->schemas.count;
    out_snapshot->contracts = fabric->contract_count;
    out_snapshot->mappings = fabric->mapping_count;
    out_snapshot->workflows = fabric->workflow_count;
    out_snapshot->revision = fabric->revision;
    umi_integration_run_history_metrics(&fabric->runs,&out_snapshot->runs);
}
