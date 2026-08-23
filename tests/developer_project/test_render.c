/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/developer_project/test_render.c
 *
 * PURPOSE:
 *   Verify deterministic {{NAME}} rendering without code execution.
 *
 * Created by: Sammy Hegab
 * Organisation: Umicom Foundation
 * Licence: MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>

#include "umicom/developer_project/render.h"

int main(void)
{
    UmiDeveloperProjectVariableSet variables;
    char output[256];

    umi_developer_project_variable_set_init(&variables);
    assert(umi_developer_project_variable_set(
        &variables, "NAME", "Umicom") == UMI_STATUS_OK);
    assert(umi_developer_project_render_text(
        "Hello {{NAME}}!",
        &variables,
        output,
        sizeof(output)) == UMI_STATUS_OK);
    assert(strcmp(output, "Hello Umicom!") == 0);

    assert(umi_developer_project_render_text(
        "{{MISSING}}",
        &variables,
        output,
        sizeof(output)) == UMI_STATUS_NOT_FOUND);
    return 0;
}
