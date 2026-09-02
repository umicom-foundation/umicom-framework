/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_cpython_extension.c
 *
 * PURPOSE:
 *   Verify the built-in CPython Native Extension project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/cpython_extension.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_cpython_extension();

    assert(project_template != NULL);
    assert(project_template->file_count == 5U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
