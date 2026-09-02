/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/data_workbench/test_transfer_workbench.c
 *
 * PURPOSE:
 *   Verify import/export plan validation and selection.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/data/workbench/transfer_workbench.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiDataTransferWorkbench workbench;
    UmiDatabaseTransferPlan plan;
    umi_data_transfer_workbench_init(&workbench);
    assert(umi_database_transfer_plan_init(
               &plan, "export-1", UMI_DATABASE_TRANSFER_EXPORT,
               UMI_DATABASE_FORMAT_CSV, "orders", "orders.csv") ==
           UMI_STATUS_OK);
    assert(umi_data_transfer_workbench_add(&workbench, &plan) == UMI_STATUS_OK);
    assert(workbench.ready && workbench.valid_count == 1U);
    assert(umi_data_transfer_workbench_select(&workbench, 0U) == UMI_STATUS_OK);
    return 0;
}
