/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_run_plan.c
 *
 * PURPOSE:
 *   Verify native/interpreter run plans preserve argv boundaries.
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

#include "umicom/developer_project/run_plan.h"

int main(void)
{
    UmiDeveloperProjectRunPlan plan;

    assert(umi_developer_project_run_plan_native(
        &plan,
        "developer.language.c23",
        "bin/app.exe",
        "C:/work/app") == UMI_STATUS_OK);
    assert(strcmp(plan.command.program, "bin/app.exe") == 0);

    assert(umi_developer_project_run_plan_interpreter(
        &plan,
        "developer.language.cpython",
        "python",
        "src/tool.py",
        "C:/work/python") == UMI_STATUS_OK);
    assert(strcmp(plan.command.program, "python") == 0);
    assert(strcmp(plan.command.arguments[0], "src/tool.py") == 0);
    return 0;
}
