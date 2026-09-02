/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/data/workbench/transfer_workbench.h
 *
 * PURPOSE:
 *   Coordinate canonical import/export plans, validation and selection without
 *   performing filesystem or database mutations in the presentation layer.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#ifndef UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H
#define UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H

#include "umicom/data/transfer_plan.h"
#include "umicom/data/workbench/types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Represent the data transfer workbench data shared with callers of this public contract.
 */
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

/**
 * Initialise data transfer workbench from caller-provided values so later operations
 * receive a known state.
 */
void umi_data_transfer_workbench_init(UmiDataTransferWorkbench *workbench);
/**
 * Add data transfer workbench only after its inputs and available capacity have been
 * checked.
 */
UmiStatus umi_data_transfer_workbench_add(
    UmiDataTransferWorkbench *workbench,
    const UmiDatabaseTransferPlan *plan);
/**
 * Provide the data transfer workbench select operation used by this module and its client
 * applications.
 */
UmiStatus umi_data_transfer_workbench_select(
    UmiDataTransferWorkbench *workbench,
    size_t plan_index);
/**
 * Provide the data transfer workbench revalidate operation used by this module and its
 * client applications.
 */
UmiStatus umi_data_transfer_workbench_revalidate(
    UmiDataTransferWorkbench *workbench);
/**
 * Find data transfer workbench plan while leaving the underlying catalogue or model owned
 * by this module.
 */
const UmiDatabaseTransferPlan *umi_data_transfer_workbench_plan_at(
    const UmiDataTransferWorkbench *workbench,
    size_t plan_index);

#ifdef __cplusplus
}
#endif

#endif /* UMICOM_DATA_WORKBENCH_TRANSFER_WORKBENCH_H */
