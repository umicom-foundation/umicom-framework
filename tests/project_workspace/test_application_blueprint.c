/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_blueprint.c
 *
 * PURPOSE:
 *   Implement the test application blueprint behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_blueprint.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceApplicationBlueprint a,b;
    CHECK(umi_project_workspace_application_blueprint_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_set_name(&a,"Application Blueprint")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_blueprint_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_application_blueprint_set_metric(&a,7U);
    CHECK(umi_project_workspace_application_blueprint_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_application_blueprint_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Application Blueprint")==0);
    return 0;
}
