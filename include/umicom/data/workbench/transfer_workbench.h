/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/transfer_workbench.h
 *
 * PURPOSE:
 *   Coordinate canonical import/export plans, validation and selection without
 *   performing filesystem or database mutations in the presentation layer.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H
#define UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H

#include "umicom/data/transfer_plan.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct UmiDataTransferWorkbench {
    uint32_t struct_size;
    uint32_t api_version;
    UmiDatabaseTransferPlan plans[UMI_DATABASE_MAX_TRANSFERS];
    size_t plan_count;
    size_t selected_index;
    size_t valid_count;
    size_t invalid_count;
    int ready;
    uint64_t revision;
} UmiDataTransferWorkbench;

void umi_data_transfer_workbench_init(UmiDataTransferWorkbench *workbench);
UmiStatus umi_data_transfer_workbench_add(
    UmiDataTransferWorkbench *workbench,
    const UmiDatabaseTransferPlan *plan);
UmiStatus umi_data_transfer_workbench_select(
    UmiDataTransferWorkbench *workbench,
    size_t plan_index);
UmiStatus umi_data_transfer_workbench_revalidate(
    UmiDataTransferWorkbench *workbench);
const UmiDatabaseTransferPlan *umi_data_transfer_workbench_plan_at(
    const UmiDataTransferWorkbench *workbench,
    size_t plan_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H */
