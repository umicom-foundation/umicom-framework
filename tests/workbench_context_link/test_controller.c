/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_context_link/test_controller.c
 *
 * PURPOSE:
 *   Verify the Workbench Context Link Slave Controller lifecycle.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/workbench_context_link/controller.h"

int main(void)
{
    UmiWorkbenchContextLinkSlaveController controller;
    umi_workbench_context_link_slave_controller_init(&controller);
    assert(controller.state == UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_INITIALISED);
    assert(umi_workbench_context_link_slave_controller_start(&controller) == UMI_STATUS_OK);
    assert(controller.state == UMI_WORKBENCH_CONTEXT_LINK_CONTROLLER_RUNNING);
    assert(umi_workbench_context_link_slave_controller_quiesce(&controller) == UMI_STATUS_OK);
    assert(controller.service.suspended);
    assert(umi_workbench_context_link_slave_controller_resume(&controller) == UMI_STATUS_OK);
    assert(!controller.service.suspended);
    assert(umi_workbench_context_link_slave_controller_stop(&controller) == UMI_STATUS_OK);
    umi_workbench_context_link_slave_controller_destroy(&controller);
    return 0;
}
