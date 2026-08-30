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
#include <assert.h>

#include "umicom/developer_project/templates/c23_console.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_c23_console();

    assert(project_template != NULL);
    assert(project_template->file_count == 7U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
