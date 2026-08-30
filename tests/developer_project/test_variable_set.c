/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_variable_set.c
 *
 * PURPOSE:
 *   Verify bounded project-template variable storage.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/variable_set.h"

int main(void)
{
    UmiDeveloperProjectVariableSet variables;

    umi_developer_project_variable_set_init(&variables);
    assert(umi_developer_project_variable_set(
        &variables, "NAME", "Alpha") == UMI_STATUS_OK);
    assert(strcmp(
        umi_developer_project_variable_get(&variables, "NAME"),
        "Alpha") == 0);
    assert(umi_developer_project_variable_set(
        &variables, "NAME", "Beta") == UMI_STATUS_OK);
    assert(strcmp(
        umi_developer_project_variable_get(&variables, "NAME"),
        "Beta") == 0);
    return 0;
}
