/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/workbench_selection_provider/test_problem_provider.c
 *
 * PURPOSE:
 *   Verify structured Problems records retain diagnostic identity and source location.
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

#include "umicom/workbench_selection_provider/problem.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    UmiUiProblemSnapshot problem = {0};
    UmiWorkbenchSelection selection;
    problem.struct_size = (uint32_t)sizeof(problem);
    problem.api_version = 1U;
    (void)strcpy(problem.id, "E001");
    (void)strcpy(problem.source, "compiler");
    (void)strcpy(problem.code, "E001");
    (void)strcpy(problem.message, "Example error");
    (void)strcpy(problem.uri, "C:/dev/main.c");
    problem.line = 42U;
    problem.column = 7U;
    problem.severity = 1;
    problem.revision = 3U;
    assert(umi_workbench_selection_provider_from_problem(
        &problem, "org.umicom.studio", "studio.problems",
        "workspace", 100U, &selection) == UMI_STATUS_OK);
    assert(selection.kind == UMI_WORKBENCH_SELECTION_DIAGNOSTIC);
    assert(selection.context_kind == UMI_CONTEXT_KIND_SOURCE_LOCATION);
    assert(strcmp(selection.path, "C:/dev/main.c") == 0);
    assert(selection.line == 42U);

    return 0;
}
