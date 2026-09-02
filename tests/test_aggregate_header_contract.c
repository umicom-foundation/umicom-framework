/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/test_aggregate_header_contract.c
 *
 * PURPOSE:
 *   Verify that the aggregate Framework header exposes the runtime inventory
 *   and compatibility contract APIs required by Framework consumers.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/umicom.h"

#include <assert.h>

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiRuntimeInventorySnapshot inventory;
    UmiRuntimeContractSnapshot contract;

    assert(umi_runtime_inventory_snapshot(NULL, NULL, NULL, &inventory) ==
           UMI_STATUS_OK);
    assert(umi_runtime_contract_snapshot(&contract) == UMI_STATUS_OK);
    assert(contract.framework_version.major == UMICOM_FRAMEWORK_VERSION_MAJOR);
    return 0;
}
