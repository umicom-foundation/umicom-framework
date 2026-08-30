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


void umi_workbench_designer_command_history_init(
    UmiWorkbenchDesignerCommandHistory *history)
{
    if (history == NULL) return;
    (void)memset(history, 0, sizeof(*history));
}

UmiStatus umi_workbench_designer_command_history_push(
    UmiWorkbenchDesignerCommandHistory *history,
    const UmiWorkbenchDesignerCommand *command,
    const UmiWorkbenchDesignerCommandResult *result)
{
    UmiWorkbenchDesignerCommandRecord *record;
    size_t index;
    if (history == NULL || command == NULL || result == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (history->count == UMI_WORKBENCH_DESIGNER_MAX_COMMANDS) {
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

const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_at(
    const UmiWorkbenchDesignerCommandHistory *history,
    size_t index)
{
    if (history == NULL || index >= history->count) return NULL;
    return &history->records[index];
}

const UmiWorkbenchDesignerCommandRecord *umi_workbench_designer_command_history_current(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    if (history == NULL || history->count == 0U) return NULL;
    return &history->records[history->count - 1U];
}

size_t umi_workbench_designer_command_history_success_count(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    size_t index;
    size_t count = 0U;
    if (history == NULL) return 0U;
    for (index = 0U; index < history->count; ++index) {
        if (history->records[index].status == UMI_STATUS_OK) count += 1U;
    }
    return count;
}

size_t umi_workbench_designer_command_history_failure_count(
    const UmiWorkbenchDesignerCommandHistory *history)
{
    if (history == NULL) return 0U;
    return history->count - umi_workbench_designer_command_history_success_count(history);
}
