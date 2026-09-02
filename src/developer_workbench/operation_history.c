/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: src/developer_workbench/operation_history.c
 *
 * PURPOSE:
 *   Implement bounded developer lifecycle evidence.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/developer_workbench/operation_history.h"

#include "umicom/base/text.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct UmiDeveloperWorkbenchOperationHistory {
    UmiDeveloperWorkbenchOperationRecord
        records[UMI_DEVELOPER_WORKBENCH_HISTORY_CAPACITY];
    size_t count;
    uint64_t sequence;
};

/*
 * Initialise developer workbench operation history from caller-provided values so later
 * operations receive a known state.
 */
UmiStatus umi_developer_workbench_operation_history_create(
    UmiDeveloperWorkbenchOperationHistory **out_history)
{
    UmiDeveloperWorkbenchOperationHistory *history;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (out_history == NULL) return UMI_STATUS_INVALID_ARGUMENT;
    *out_history = NULL;

    history = (UmiDeveloperWorkbenchOperationHistory *)calloc(
        1U, sizeof(*history));
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL) return UMI_STATUS_OUT_OF_MEMORY;

    *out_history = history;
    return UMI_STATUS_OK;
}

/*
 * Release or reset state held by developer workbench operation history so the same storage
 * can be reused safely.
 */
void umi_developer_workbench_operation_history_destroy(
    UmiDeveloperWorkbenchOperationHistory *history)
{
    free(history);
}

/*
 * Add developer workbench operation history only after its inputs and available capacity
 * have been checked.
 */
UmiStatus umi_developer_workbench_operation_history_append(
    UmiDeveloperWorkbenchOperationHistory *history,
    const char *command_id,
    UmiBuildAction action,
    UmiStatus status,
    const UmiBuildReport *report)
{
    UmiDeveloperWorkbenchOperationRecord *record;
    size_t index;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || command_id == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }

    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (history->count >= UMI_DEVELOPER_WORKBENCH_HISTORY_CAPACITY) {
        /* Visit each bounded item once so every record receives the same rule. */
        for (index = 1U; index < history->count; ++index) {
            history->records[index - 1U] = history->records[index];
        }
        history->count -= 1U;
    }

    record = &history->records[history->count++];
    (void)memset(record, 0, sizeof(*record));
    record->sequence = ++history->sequence;
    (void)umi_text_copy_truncated(record->command_id,
                                  sizeof(record->command_id),
                                  command_id);
    record->action = action;
    record->status = status;

    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
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
        (void)umi_text_copy_truncated(record->summary,
                                      sizeof(record->summary),
                                      report->last_output);
    }

    return UMI_STATUS_OK;
}

/*
 * Find developer workbench operation history while leaving the underlying catalogue or
 * model owned by this module.
 */
UmiStatus umi_developer_workbench_operation_history_at(
    const UmiDeveloperWorkbenchOperationHistory *history,
    size_t index,
    UmiDeveloperWorkbenchOperationRecord *out_record)
{
    /*
     * Protect caller-owned memory by checking that required state is available before it is
     * used.
     */
    if (history == NULL || out_record == NULL) {
        return UMI_STATUS_INVALID_ARGUMENT;
    }
    /* Keep the operation inside its valid bounds before reading, writing or adding data. */
    if (index >= history->count) return UMI_STATUS_NOT_FOUND;

    *out_record = history->records[index];
    return UMI_STATUS_OK;
}

/*
 * Return the number of records represented by developer workbench operation history
 * without changing their state.
 */
size_t umi_developer_workbench_operation_history_count(
    const UmiDeveloperWorkbenchOperationHistory *history)
{
    return history != NULL ? history->count : 0U;
}
