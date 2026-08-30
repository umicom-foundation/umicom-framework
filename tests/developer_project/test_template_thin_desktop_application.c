/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_thin_desktop_application.c
 *
 * PURPOSE:
 *   Verify the built-in Umicom Thin Desktop Application project-template contract.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/thin_desktop_application.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_thin_desktop_application();

    assert(project_template != NULL);
    assert(project_template->file_count == 6U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
