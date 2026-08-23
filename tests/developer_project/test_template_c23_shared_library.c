/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_c23_shared_library.c
 *
 * PURPOSE:
 *   Verify the built-in C23 Shared Library project-template contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/c23_shared_library.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_c23_shared_library();

    assert(project_template != NULL);
    assert(project_template->file_count == 6U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
