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


/**
 * Represent the workbench designer controller data shared with callers of this public
 * contract.
 */
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

/**
 * Initialise workbench designer controller from caller-provided values so later operations
 * receive a known state.
 */
void umi_workbench_designer_controller_init(UmiWorkbenchDesignerController *controller, const char *controller_id, UmiWorkbenchDesignerService *service);
/**
 * Provide the workbench designer controller initialise operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_controller_initialise(UmiWorkbenchDesignerController *controller);
/**
 * Provide the workbench designer controller start operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_controller_start(UmiWorkbenchDesignerController *controller);
/**
 * Provide the workbench designer controller quiesce operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_controller_quiesce(UmiWorkbenchDesignerController *controller);
/**
 * Provide the workbench designer controller stop operation used by this module and its
 * client applications.
 */
UmiStatus umi_workbench_designer_controller_stop(UmiWorkbenchDesignerController *controller);
/**
 * Perform workbench designer controller through the module contract so client applications
 * do not duplicate its policy.
 */
UmiStatus umi_workbench_designer_controller_dispatch(UmiWorkbenchDesignerController *controller, const UmiWorkbenchDesignerControllerCommand *command, UmiWorkbenchDesignerControllerResult *out_result);

#ifdef __cplusplus
}
#endif

#endif
