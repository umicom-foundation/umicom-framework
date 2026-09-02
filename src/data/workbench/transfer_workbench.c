/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/transfer_workbench.c
 *
 * PURPOSE:
 *   Implement validation and selection over canonical transfer plans.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/transfer_workbench.h"

#include <string.h>

/*
 * Initialise data transfer workbench from caller-provided values so later operations
 * receive a known state.
 */
void umi_data_transfer_workbench_init(UmiDataTransferWorkbench *workbench)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return;
    (void)memset(workbench, 0, sizeof(*workbench));
    workbench->struct_size = (uint32_t)sizeof(*workbench);
    workbench->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    workbench->revision = 1U;
}

/*
 * Provide the data transfer workbench revalidate operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_transfer_workbench_revalidate(
    UmiDataTransferWorkbench *workbench)
{
    size_t index;
    char message[UMI_DATABASE_TEXT_CAPACITY];
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench->valid_count = 0U;
    workbench->invalid_count = 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workbench->plan_count; ++index) {
        /* Apply this operation only while the related capability or state is available. */
        if (umi_database_transfer_plan_validate(
                &workbench->plans[index], message, sizeof(message)) ==
            UMI_STATUS_OK) {
            workbench->valid_count += 1U;
        } /* Use this fallback path when the earlier condition does not apply. */ else {
            workbench->invalid_count += 1U;
        }
    }
    workbench->ready = workbench->plan_count > 0U &&
        workbench->valid_count == workbench->plan_count;
    workbench->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Add data transfer workbench only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_data_transfer_workbench_add(
    UmiDataTransferWorkbench *workbench,
    const UmiDatabaseTransferPlan *plan)
{
    size_t index;
    char message[UMI_DATABASE_TEXT_CAPACITY];
    UmiStatus status;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_database_transfer_plan_validate(plan, message, sizeof(message));
    /* Preserve the original failure result so the caller can respond to the correct cause. */
    if (status != UMI_STATUS_OK) return status;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < workbench->plan_count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (strcmp(workbench->plans[index].id, plan->id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (workbench->plan_count >= UMI_DATABASE_MAX_TRANSFERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    workbench->plans[workbench->plan_count++] = *plan;
    return umi_data_transfer_workbench_revalidate(workbench);
}

/*
 * Provide the data transfer workbench select operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_transfer_workbench_select(
    UmiDataTransferWorkbench *workbench,
    size_t plan_index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (plan_index >= workbench->plan_count) return UMI_STATUS_NOT_FOUND;
    workbench->selected_index = plan_index;
    workbench->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find data transfer workbench plan while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDatabaseTransferPlan *umi_data_transfer_workbench_plan_at(
    const UmiDataTransferWorkbench *workbench,
    size_t plan_index)
{
    return workbench != NULL && plan_index < workbench->plan_count
        ? &workbench->plans[plan_index] : NULL;
}
