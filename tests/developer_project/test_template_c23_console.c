/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_c23_console.c
 *
 * PURPOSE:
 *   Verify the built-in C23 Console Application project-template contract.
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

#include "umicom/developer_project/templates/c23_console.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_c23_console();

    assert(project_template != NULL);
    assert(project_template->file_count == 8U);
    assert(project_template->supports_tests);
    assert(strstr(project_template->files[0].content_template, "$<TARGET_FILE:{{TARGET_NAME}}>") != NULL);
    assert(strstr(project_template->files[0].content_template,
                  "-DEXPECTED_FILE=${CMAKE_CURRENT_SOURCE_DIR}/tests/expected-output.txt") != NULL);
    assert(strcmp(project_template->files[3].relative_path, "tests/check_console.cmake") == 0);
    assert(strcmp(project_template->files[4].relative_path, "tests/expected-output.txt") == 0);
    assert(strcmp(project_template->files[4].content_template, "{{APPLICATION_NAME}}\n") == 0);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
