#include "umicom/project/workspace/project_metadata.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceProjectMetadata a,b;
    CHECK(umi_project_workspace_project_metadata_init(&a,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_init(&b,"item")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_set_name(&a,"Project Metadata")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_project_metadata_set_detail(&a,"framework-owned")==UMI_STATUS_OK);
    umi_project_workspace_project_metadata_set_metric(&a,7U);
    CHECK(umi_project_workspace_project_metadata_validate(&a)==UMI_STATUS_OK);
    CHECK(a.metric==7U);
    CHECK(umi_project_workspace_project_metadata_same_identity(&a,&b));
    CHECK(strcmp(a.base.name,"Project Metadata")==0);
    return 0;
}
