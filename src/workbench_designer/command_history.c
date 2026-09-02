/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/workbench_designer/command_history.c
 *
 * PURPOSE:
 *   Implement bounded command evidence with deterministic oldest-record
 *   eviction.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "umicom/workbench_designer/command_history.h"
#include "internal.h"


/*
 * Initialise workbench designer command history from caller-provided values so later
 * operations receive a known state.
 */
void umi_workbench_designer_command_history_init(
    UmiWorkbenchDesignerCommandHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return;
    (void)memset(history, 0, sizeof(*history));
}

/*
 * Provide the workbench designer command history push operation used by this module and
 * its client applications.
 */
UmiStatus umi_workbench_designer_command_history_push(
    UmiWorkbenchDesignerCommandHistory *history,
    const UmiWorkbenchDesignerCommand *command,
    const UmiWorkbenchDesignerCommandResult *result)
{
    UmiWorkbenchDesignerCommandRecord *record;
    size_t index;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || command == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count == UMI_WORKBENCH_DESIGNER_MAX_COMMANDS) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 1U; index < history->count; ++index) {
            history->records[index - 1U] = history->records[index];
        }
        history->count -= 1U;
    }
    record = &history->records[history->count++];
    (void)memset(record, 0, sizeof(*record));
    record->command = *command;
    record->status = result->status;
    record->before_revision = result->previous_revision;
    record->after_revision = result->resulting_revision;
    (void)umi_workbench_designer_copy_text(
        record->message, sizeof(record->message), result->message);
    history->cursor = history->count;
    history->revision += 1U;
    return UMI_STATUS_OK;
}

/*
 * Find workbench designer command history while leaving the underlying catalogue or model
 * owned by this module.
 */
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_at(
    const UmiWorkbenchDesignerCommandHistory *history,
    size_t index)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || index >= history->count) return NULL;
    return &history->records[index];
}

/*
 * Provide the workbench designer command history current operation used by this module and
 * its client applications.
 */
const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_current(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || history->count == 0U) return NULL;
    return &history->records[history->count - 1U];
}

/*
 * Return the number of records represented by workbench designer command history success
 * without changing their state.
 */
size_t umi_workbench_designer_command_history_success_count(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    size_t index;
    size_t count = 0U;
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return 0U;
    /* Visit each bounded item once so every record receives the same rule. */
    for (index = 0U; index < history->count; ++index) {
        /* Keep the operation inside its valid bounds before reading, writing or adding data. */
        if (history->records[index].status == UMI_STATUS_OK) count += 1U;
    }
    return count;
}

/*
 * Return the number of records represented by workbench designer command history failure
 * without changing their state.
 */
size_t umi_workbench_designer_command_history_failure_count(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return 0U;
    return history->count - umi_workbench_designer_command_history_success_count(history);
}
