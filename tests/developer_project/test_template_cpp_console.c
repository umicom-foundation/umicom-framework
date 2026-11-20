/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_cpp_console.c
 *
 * PURPOSE:
 *   Verify the built-in C++ Console Application project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
/* Required fixture setup and checks also execute in optimized builds. */
#include "../support/assertions.h"
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/templates/cpp_console.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_cpp_console();

    assert(project_template != NULL);
    /* The extra file checks the real console output. Retain the older
     * five-file contract for review of previously generated projects. */
#if 0
    assert(project_template->file_count == 5U);
#endif
    assert(project_template->file_count == 6U && project_template->supports_tests);
    assert(strstr(project_template->files[0].content_template, "$<TARGET_FILE:{{TARGET_NAME}}>") != NULL);
    assert(strcmp(project_template->files[3].relative_path, "tests/check_console.cmake") == 0);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
