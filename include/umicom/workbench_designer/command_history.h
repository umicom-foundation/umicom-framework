/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: include/umicom/workbench_designer/command_history.h
 *
 * PURPOSE:
 *   Retain bounded designer command evidence independently from the document
 *   snapshot history used for undo and redo.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#ifndef UMICOM_WORKBENCH_DESIGNER_COMMAND_HISTORY_H
#define UMICOM_WORKBENCH_DESIGNER_COMMAND_HISTORY_H

#include "umicom/workbench_designer/command.h"

#ifdef __cplusplus
extern "C" {
#endif


/**
 * Represent the workbench designer command record data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandRecord {
    UmiWorkbenchDesignerCommand command;
    UmiStatus status;
    uint64_t before_revision;
    uint64_t after_revision;
    char message[UMI_WORKBENCH_DESIGNER_TEXT_CAPACITY];
} UmiWorkbenchDesignerCommandRecord;

/**
 * Represent the workbench designer command history data shared with callers of this public
 * contract.
 */
typedef struct UmiWorkbenchDesignerCommandHistory {
    UmiWorkbenchDesignerCommandRecord records[UMI_WORKBENCH_DESIGNER_MAX_COMMANDS];
    size_t count;
    size_t cursor;
    uint64_t revision;
} UmiWorkbenchDesignerCommandHistory;

/**
 * Initialise workbench designer command history from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_command_history_init(UmiWorkbenchDesignerCommandHistory *history);
/**
 * Provide the workbench designer command history push operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_command_history_push(UmiWorkbenchDesignerCommandHistory *history, const UmiWorkbenchDesignerCommand *command, const UmiWorkbenchDesignerCommandResult *result);
/**
 * Find workbench designer command history while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_at(const UmiWorkbenchDesignerCommandHistory *history, size_t index);
/**
 * Provide the workbench designer command history current operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_current(const UmiWorkbenchDesignerCommandHistory *history);
/**
 * Return the number of records represented by workbench designer command history success
 * without changing their state.
 */
size_t umi_workbench_designer_command_history_success_count(const UmiWorkbenchDesignerCommandHistory *history);
/**
 * Return the number of records represented by workbench designer command history failure
 * without changing their state.
 */
size_t umi_workbench_designer_command_history_failure_count(const UmiWorkbenchDesignerCommandHistory *history);

#ifdef __cplusplus
}
#endif

#endif
