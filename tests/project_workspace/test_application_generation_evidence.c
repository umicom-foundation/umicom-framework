/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_application_generation_evidence.c
 *
 * PURPOSE:
 *   Implement the test application generation evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/application_generation_evidence.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    uint64_t a=umi_project_workspace_application_generation_evidence_fingerprint("plan");
    CHECK(a!=0U);
    CHECK(a==umi_project_workspace_application_generation_evidence_fingerprint("plan"));
    CHECK(a!=umi_project_workspace_application_generation_evidence_fingerprint("other"));
    return 0;
}
