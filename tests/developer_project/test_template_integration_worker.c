/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_integration_worker.c
 *
 * PURPOSE:
 *   Verify the built-in Umicom Integration Worker project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/integration_worker.h"

/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_integration_worker();

    assert(project_template != NULL);
    assert(project_template->file_count == 6U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
