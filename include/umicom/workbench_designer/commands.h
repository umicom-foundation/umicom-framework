/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/commands.h
 *
 * PURPOSE:
 *   Define typed commands routed from the Master Controller to the Layout
 *   Designer Slave Controller.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMANDS_H
#define UMICOM_WORKBENCH_DESIGNER_COMMANDS_H

#include "umicom/workbench_designer/command.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef enum UmiWorkbenchDesignerControllerCommandKind {
    UMI_WORKBENCH_DESIGNER_CONTROLLER_START = 1,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_QUIESCE = 2,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_STOP = 3,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_OPEN = 4,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_CLOSE = 5,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_ACTIVATE = 6,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_APPLY = 7,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_SET_MODE = 8,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_SET_TOOL = 9,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_SELECT = 10,
    UMI_WORKBENCH_DESIGNER_CONTROLLER_AUTOSAVE = 11
} UmiWorkbenchDesignerControllerCommandKind;

typedef struct UmiWorkbenchDesignerControllerCommand {
    UmiWorkbenchDesignerControllerCommandKind kind;
    char command_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char actor_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char correlation_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    UmiWorkbenchDesignerCommand designer_command;
    UmiWorkbenchDesignerMode mode;
    UmiWorkbenchDesignerTool tool;
    bool discard_dirty;
    bool extend_selection;
    bool toggle_selection;
    uint64_t timestamp_ms;
} UmiWorkbenchDesignerControllerCommand;

typedef struct UmiWorkbenchDesignerControllerResult {
    UmiStatus status;
    bool changed;
    char session_id[UMI_WORKBENCH_DESIGNER_ID_CAPACITY];
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
    uint64_t service_revision;
} UmiWorkbenchDesignerControllerResult;

void umi_workbench_designer_controller_command_init(UmiWorkbenchDesignerControllerCommand *command, UmiWorkbenchDesignerControllerCommandKind kind, const char *command_id);
UmiStatus umi_workbench_designer_controller_command_validate(const UmiWorkbenchDesignerControllerCommand *command);

#ifdef __cplusplus
}
#endif

#endif
