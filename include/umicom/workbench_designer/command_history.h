/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command_history.h
 *
 * PURPOSE:
 *   Retain bounded designer command evidence independently from the document
 *   snapshot history used for undo and redo.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_HISTORY_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_HISTORY_H

#include "umicom/workbench_designer/command.h"

#ifdef __cplusplus
extern "C" {
#endif


typedef struct UmiWorkbenchDesignerCommandRecord {
    UmiWorkbenchDesignerCommand command;
    UmiStatus status;
    uint64_t before_revision;
    uint64_t after_revision;
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerCommandRecord;

typedef struct UmiWorkbenchDesignerCommandHistory {
    UmiWorkbenchDesignerCommandRecord records[UMI_WORKBENCH_DESIGNER_MAX_COMMANDS];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchDesignerCommandHistory;

void umi_workbench_designer_command_history_init(UmiWorkbenchDesignerCommandHistory *history);
UmiStatus umi_workbench_designer_command_history_push(UmiWorkbenchDesignerCommandHistory *history, const UmiWorkbenchDesignerCommand *command, const UmiWorkbenchDesignerCommandResult *result);
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_at(const UmiWorkbenchDesignerCommandHistory *history, size_t index);
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_current(const UmiWorkbenchDesignerCommandHistory *history);
size_t umi_workbench_designer_command_history_success_count(const UmiWorkbenchDesignerCommandHistory *history);
size_t umi_workbench_designer_command_history_failure_count(const UmiWorkbenchDesignerCommandHistory *history);

#ifdef __cplusplus
}
#endif

#endif
