/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/web_workbench/test_environment.c
 * PURPOSE: Verify public environment values and unresolved placeholder evidence.
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include <assert.h>
#include <string.h>
#include "umicom/web/workbench/environment.h"

int main(void)
{
    UmiWebWorkbenchEnvironment environment;
    char resolved[256U];
    size_t unresolved = 0U;
    umi_web_workbench_environment_init(&environment, "dev", "Development");
    assert(umi_web_workbench_environment_set(&environment, "baseUrl",
        "https://api.example.test", true) == UMI_STATUS_OK);
    assert(umi_web_workbench_environment_set(&environment, "disabled", "x", false) == UMI_STATUS_OK);
    assert(umi_web_workbench_environment_resolve(&environment,
        "{{baseUrl}}/v1/{{missing}}/{{disabled}}", resolved, sizeof(resolved),
        &unresolved) == UMI_STATUS_OK);
    assert(strcmp(resolved, "https://api.example.test/v1/{{missing}}/{{disabled}}") == 0);
    assert(unresolved == 2U);
    assert(umi_web_workbench_environment_remove(&environment, "baseUrl") == UMI_STATUS_OK);
    assert(umi_web_workbench_environment_find(&environment, "baseUrl") == NULL);
    return 0;
}
