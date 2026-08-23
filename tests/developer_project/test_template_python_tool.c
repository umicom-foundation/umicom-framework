/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_template_python_tool.c
 *
 * PURPOSE:
 *   Verify the built-in Python Tool project-template contract.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>

#include "umicom/developer_project/templates/python_tool.h"

int main(void)
{
    const UmiDeveloperProjectTemplate *project_template =
        umi_developer_project_template_python_tool();

    assert(project_template != NULL);
    assert(project_template->file_count == 4U);
    assert(umi_developer_project_template_validate(project_template) ==
           UMI_STATUS_OK);
    return 0;
}
