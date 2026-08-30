/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_zig_console.c
 *
 * PURPOSE:
 *   Verify the built-in Zig Console Application project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/zig_console.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_zig_console();

    assert(project_template != NULL);
    assert(project_template->file_count == 4U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
