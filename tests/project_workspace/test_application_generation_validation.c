/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_generation_validation.c
 *
 * PURPOSE:
 *   Implement the test application generation validation behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_generation_validation.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceGenerationInput i= {
        "app","thin-desktop","C:/code",1U,1U
    };
    char m[64];
    CHECK(umi_project_workspace_application_generation_validation_evaluate(&i,m,sizeof(m))==UMI_STATUS_OK);
    CHECK(strcmp(m,"ready")==0);
    return 0;
}
