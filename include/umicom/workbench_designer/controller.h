/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/controller.h
 *
 * PURPOSE:
 *   Implement the Layout Designer Slave Controller lifecycle and typed command
 *   dispatch under Master Controller authority.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_CONTROLLER_H
#define UMICOM_WORKBENCH_DESIGNER_CONTROLLER_H

#include "umicom/workbench_designer/commands.h"
#include "umicom/workbench_designer/service.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerController {
    UmiWorkbenchDesignerState state;
    UmiWorkbenchDesignerService *service;
    char controller_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    uint64_t handled_command_count;
    uint64_t failed_command_count;
    UmiStatus last_status;
    char last_error[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t revision;
} UmiWorkbenchDesignerController;

void umi_workbench_designer_controller_init(UmiWorkbenchDesignerController *controller, const char *controller_id, UmiWorkbenchDesignerService *service);
UmiStatus umi_workbench_designer_controller_initialise(UmiWorkbenchDesignerController *controller);
UmiStatus umi_workbench_designer_controller_start(UmiWorkbenchDesignerController *controller);
UmiStatus umi_workbench_designer_controller_quiesce(UmiWorkbenchDesignerController *controller);
UmiStatus umi_workbench_designer_controller_stop(UmiWorkbenchDesignerController *controller);
UmiStatus umi_workbench_designer_controller_dispatch(UmiWorkbenchDesignerController *controller, const UmiWorkbenchDesignerControllerCommand *command, UmiWorkbenchDesignerControllerResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
