/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/development_workflow/test_preview_output.c
 *
 * PURPOSE:
 *   Verify read-only workflow preview refuses buffered-output failures without executing planned commands.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/

#include "../support/report_stream.h"
#define printf ReportTestPrintf
#define puts ReportTestPuts
#define fflush ReportTestFlush
#define ferror ReportTestError
#define main UmiTestWorkflowExampleMain
#include "../../examples/development_workflow/main.c"
#undef main
#undef printf
#undef puts
#undef fflush
#undef ferror
#undef CHECK

int main(int argc, char **argv)
{
    REPORT_CHECK(argc == 2);
    char *arguments[] = {"umicom-development-workflow-example", "preview",
        "source preview only", "build preview only", "Release", "Ninja"};
    ReportTestReset("success", 0U);
    REPORT_CHECK(UmiTestWorkflowExampleMain(6, arguments) == EXIT_SUCCESS);
    REPORT_CHECK(strstr(ReportTestText, "\"mode\":\"preview\"") != NULL);
    REPORT_CHECK(strstr(ReportTestText, "\"executed\":false") != NULL);
    REPORT_CHECK(strstr(ReportTestText, "\"--config\",\"Release\"") != NULL);
    REPORT_CHECK(ReportTestWrites == 1U && ReportTestFlushes == 1U && ReportTestErrors == 1U);
    size_t report_size = ReportTestLength;
    char *complete = malloc(report_size + 1U); REPORT_CHECK(complete != NULL);
    memcpy(complete, ReportTestText, report_size + 1U);
    ReportTestReset(argv[1], 1U);
    REPORT_CHECK(UmiTestWorkflowExampleMain(6, arguments) ==
        (strcmp(argv[1], "success") == 0 ? EXIT_SUCCESS : EXIT_FAILURE));
    REPORT_CHECK(ReportTestWrites == 1U && ReportTestFlushes == 1U && ReportTestErrors == 1U);
    ReportTestReset("success", 0U);
    REPORT_CHECK(UmiTestWorkflowExampleMain(6, arguments) == EXIT_SUCCESS);
    REPORT_CHECK(ReportTestLength == report_size && strcmp(ReportTestText, complete) == 0);
    free(complete);
    return 0;
}
