/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_cpp_gtk4.c
 *
 * PURPOSE:
 *   Verify the built-in C++ GTK4 Application project-template contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/cpp_gtk4.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_cpp_gtk4();

    assert(project_template != NULL);
    assert(project_template->file_count == 5U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
