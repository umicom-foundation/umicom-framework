/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/data/workbench/transfer_workbench.c
 *
 * PURPOSE:
 *   Implement validation and selection over canonical transfer plans.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/data/workbench/transfer_workbench.h"

#include <string.h>

void umi_data_transfer_workbench_init(UmiDataTransferWorkbench *workbench)
{
    if (workbench == NULL) return;
    (void)memset(workbench, 0, sizeof(*workbench));
    workbench->struct_size = (uint32_t)sizeof(*workbench);
    workbench->api_version = UMI_DATA_WORKBENCH_API_VERSION;
    workbench->revision = 1U;
}

UmiStatus umi_data_transfer_workbench_revalidate(
    UmiDataTransferWorkbench *workbench)
{
    size_t index;
    char message[UMI_DATABASE_TEXT_CAPACITY];
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    workbench->valid_count = 0U;
    workbench->invalid_count = 0U;
    for (index = 0U; index < workbench->plan_count; ++index) {
        if (umi_database_transfer_plan_validate(
                &workbench->plans[index], message, sizeof(message)) ==
            UMI_STATUS_OK) {
            workbench->valid_count += 1U;
        } else {
            workbench->invalid_count += 1U;
        }
    }
    workbench->ready = workbench->plan_count > 0U &&
        workbench->valid_count == workbench->plan_count;
    workbench->revision += 1U;
    return UMI_STATUS_OK;
}

UmiStatus umi_data_transfer_workbench_add(
    UmiDataTransferWorkbench *workbench,
    const UmiDatabaseTransferPlan *plan)
{
    size_t index;
    char message[UMI_DATABASE_TEXT_CAPACITY];
    UmiStatus status;
    if (workbench == NULL || plan == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    status = umi_database_transfer_plan_validate(plan, message, sizeof(message));
    if (status != UMI_STATUS_OK) return status;
    for (index = 0U; index < workbench->plan_count; ++index) {
        if (strcmp(workbench->plans[index].id, plan->id) == 0) {
            return UMI_STATUS_ALREADY_EXISTS;
        }
    }
    if (workbench->plan_count >= UMI_DATABASE_MAX_TRANSFERS) {
        return UMI_STATUS_CAPACITY_EXCEEDED;
    }
    workbench->plans[workbench->plan_count++] = *plan;
    return umi_data_transfer_workbench_revalidate(workbench);
}

UmiStatus umi_data_transfer_workbench_select(
    UmiDataTransferWorkbench *workbench,
    size_t plan_index)
{
    if (workbench == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    if (plan_index >= workbench->plan_count) return UMI_STATUS_NOT_FOUND;
    workbench->selected_index = plan_index;
    workbench->revision += 1U;
    return UMI_STATUS_OK;
}

const UmiDatabaseTransferPlan *umi_data_transfer_workbench_plan_at(
    const UmiDataTransferWorkbench *workbench,
    size_t plan_index)
{
    return workbench != NULL && plan_index < workbench->plan_count
        ? &workbench->plans[plan_index] : NULL;
}
