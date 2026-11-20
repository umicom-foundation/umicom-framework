/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_c23_assembly.c
 *
 * PURPOSE:
 *   Verify the built-in C23 + Assembly Application project-template contract.
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

#include "umicom/developer_project/templates/c23_assembly.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_c23_assembly();

    assert(project_template != NULL);
    /* The original inventory had six files and no executable test recipe.
     * Keep that expectation for comparison with the added native check. */
#if 0
    assert(project_template->file_count == 6U);
#endif
    assert(project_template->file_count == 7U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    /* Inspect registration and expected output separately so a template
     * cannot advertise Tests while emitting an empty CTest suite. */
    assert(strstr(project_template->files[0].content_template, "include(CTest)") != NULL);
    assert(strstr(project_template->files[0].content_template, "-DEXPECTED=Assembly answer: 42") != NULL);
    assert(strstr(project_template->files[0].content_template, "umicom_apply_application_branding") != NULL);
    assert(strcmp(project_template->files[4].relative_path, "tests/check_console.cmake") == 0);
    assert(strstr(project_template->files[4].content_template, "execute_process(") != NULL);
    return 0;
}
