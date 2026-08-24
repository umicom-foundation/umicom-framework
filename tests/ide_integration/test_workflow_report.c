/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/ide_integration/test_workflow_report.c
 *
 * PURPOSE:
 *   Focused regression coverage for IDE integration workflow report.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/

#include <assert.h>
#include "umicom/ide_integration/workflow_report.h"
int main(void)
{
    UmiIdeWorkflowReport report;
    umi_ide_workflow_report_init(&report);
    assert(report.ready);
    assert(umi_ide_workflow_report_add(
        &report, "a", "A", "ok", UMI_IDE_GATE_PASS, 1) == UMI_STATUS_OK);
    assert(umi_ide_workflow_report_add(
        &report, "b", "B", "bad", UMI_IDE_GATE_BLOCK, 1) == UMI_STATUS_OK);
    assert(!report.ready);
    assert(report.blocked_count == 1U);
    return 0;
}

