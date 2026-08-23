/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/operation_history.c
 *
 * PURPOSE:
 *   Implement bounded developer lifecycle evidence.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/operation_history.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchOperationHistory {
    UmiDeveloperWorkbenchOperationRecord
        records[UMI_DEVELOPER_WORKBENCH_HISTORY_CAPACITY];
    size_t count;
    uint64_t sequence;
};

UmiStatus umi_developer_workbench_operation_history_create(
    UmiDeveloperWorkbenchOperationHistory **out_history)
{
    UmiDeveloperWorkbenchOperationHistory *history;

    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiDeveloperWorkbenchOperationHistory *)calloc(
        1U, sizeof(*history));
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

void umi_developer_workbench_operation_history_destroy(
    UmiDeveloperWorkbenchOperationHistory *history)
{
    free(history);
}

UmiStatus umi_developer_workbench_operation_history_append(
    UmiDeveloperWorkbenchOperationHistory *history,
    const char *command_id,
    UmiBuildAction action,
    UmiStatus status,
    const UmiBuildReport *report)
{
    UmiDeveloperWorkbenchOperationRecord *record;
    size_t index;

    if (history == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    if (history->count >= UMI_DEVELOPER_WORKBENCH_HISTORY_CAPACITY) {
        for (index = 1U; index < history->count; ++index) {
            history->records[index - 1U] = history->records[index];
        }
        history->count -= 1U;
    }

    record = &history->records[history->count++];
    (void)memset(record, 0, sizeof(*record));
    record->sequence = ++history->sequence;
    (void)snprintf(record->command_id,
                   sizeof(record->command_id),
                   "%s",
                   command_id);
    record->action = action;
    record->status = status;

    if (report != NULL) {
        record->exit_code =
            action == UMI_BUILD_CONFIGURE ? report->configure_exit_code :
            action == UMI_BUILD_TEST ? report->test_exit_code :
            action == UMI_BUILD_RUN ? report->run_exit_code :
            action == UMI_BUILD_INSTALL ? report->install_exit_code :
            action == UMI_BUILD_PACKAGE ? report->package_exit_code :
            report->build_exit_code;
        record->cancelled = report->cancelled;
        record->timed_out = report->timed_out;
        record->duration_ms = report->duration_ms;
        (void)snprintf(record->summary,
                       sizeof(record->summary),
                       "%s",
                       report->last_output);
    }

    return UMI_STATUS_OK;
}

UmiStatus umi_developer_workbench_operation_history_at(
    const UmiDeveloperWorkbenchOperationHistory *history,
    size_t index,
    UmiDeveloperWorkbenchOperationRecord *out_record)
{
    if (history == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_record = history->records[index];
    return UMI_STATUS_OK;
}

size_t umi_developer_workbench_operation_history_count(
    const UmiDeveloperWorkbenchOperationHistory *history)
{
    return history != NULL ? history->count : 0U;
}
