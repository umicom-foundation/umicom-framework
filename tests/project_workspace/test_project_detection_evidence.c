#include "umicom/project/workspace/project_detection_evidence.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
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
