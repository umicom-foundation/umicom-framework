/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_generator_request.c
 *
 * PURPOSE:
 *   Implement the test application generator request behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_generator_request.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceApplicationGeneratorRequest a,b;
    CHECK(umi_project_workspace_application_generator_request_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_set_name(&a,"Application Generator Request")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_application_generator_request_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_application_generator_request_set_metric(&a,7U);
    CHECK(umi_project_workspace_application_generator_request_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_application_generator_request_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Application Generator Request")==0);
    return 0;
}
