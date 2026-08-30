/*-----------------------------------------------------------------------------
 * Umicom Framework
 * File: tests/project_workspace/test_project_import_candidate.c
 *
 * PURPOSE:
 *   Implement the test project import candidate behavior for
 *   Umicom Framework.
 *
 * AUTHOR AND ORGANISATION:
 * Sammy Hegab
 * Umicom Foundation
 *
 * LICENCE:
 * MIT
 *---------------------------------------------------------------------------*/
#include "umicom/project/workspace/project_import_candidate.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectImportCandidate a,b;
    CHECK(umi_project_workspace_project_import_candidate_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_import_candidate_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_import_candidate_set_name(&a,"Project Import Candidate")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_import_candidate_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_import_candidate_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_import_candidate_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_import_candidate_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Import Candidate")==0);
    return 0;
}
