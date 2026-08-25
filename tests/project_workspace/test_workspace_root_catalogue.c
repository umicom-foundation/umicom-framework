#include "umicom/project/workspace/workspace_root_catalogue.h"
#include <string.h>
#define CHECK(x) do { if (!(x)) return __LINE__; } while (0)
int main(void) {
    UmiProjectWorkspaceWorkspaceRootCatalogue v;
    const char *id = NULL;
    umi_project_workspace_workspace_root_catalogue_init(&v);
    CHECK(umi_project_workspace_workspace_root_catalogue_add(&v,"alpha")==UMI_STATUS_OK);
    CHECK(umi_project_workspace_workspace_root_catalogue_add(&v,"alpha")==UMI_STATUS_ALREADY_EXISTS);
    CHECK(umi_project_workspace_workspace_root_catalogue_contains(&v,"alpha"));
    CHECK(umi_project_workspace_workspace_root_catalogue_at(&v,0U,&id)==UMI_STATUS_OK);
    CHECK(strcmp(id,"alpha")==0);
    return 0;
}
