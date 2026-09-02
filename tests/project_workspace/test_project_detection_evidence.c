/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_detection_evidence.c
 *
 * PURPOSE:
 *   Implement the test project detection evidence behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_detection_evidence.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
/*
 * Start this command or application, report setup failures, and return a process exit code
 * to the operating system.
 */
int main(void) {
    UmiProjectWorkspaceProjectDetectionEvidence a,b;
    CHECK(umi_project_workspace_project_detection_evidence_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_detection_evidence_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_detection_evidence_set_name(&a,"Project Detection Evidence")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_detection_evidence_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_detection_evidence_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_detection_evidence_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_detection_evidence_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Detection Evidence")==0);
    return 0;
}
