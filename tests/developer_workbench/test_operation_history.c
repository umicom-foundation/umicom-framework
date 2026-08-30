/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_workbench/test_operation_history.c
 *
 * PURPOSE:
 *   Verify lifecycle evidence is retained by value.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_workbench/operation_history.h"

int main(void)
{
    UmiDeveloperWorkbenchOperationHistory *history = NULL;
    UmiDeveloperWorkbenchOperationRecord record;
    UmiBuildReport report;

    umi_build_report_init(&report);
    report.build_exit_code = 0;
    report.duration_ms = 42U;
    (void)strcpy(report.last_output, "Build succeeded.");

    assert(umi_developer_workbench_operation_history_create(&history) ==
           UMI_STATUS_OK);
    assert(umi_developer_workbench_operation_history_append(
        history,
        "build.build",
        UMI_BUILD_COMPILE,
        UMI_STATUS_OK,
        &report) == UMI_STATUS_OK);
    assert(umi_developer_workbench_operation_history_count(history) == 1U);
    assert(umi_developer_workbench_operation_history_at(
        history, 0U, &record) == UMI_STATUS_OK);
    assert(record.duration_ms == 42U);
    assert(strcmp(record.command_id, "build.build") == 0);

    umi_developer_workbench_operation_history_destroy(history);
    return 0;
}
