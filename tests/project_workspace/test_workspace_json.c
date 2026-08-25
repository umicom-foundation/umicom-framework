#include "umicom/project/workspace/workspace_json.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceWorkspaceSnapshot s;
    char j[512];
    CHECK(umi_project_workspace_workspace_snapshot_init(&s,"w")==UMI_STATUS_OK);
    s.project_count=3U;
    CHECK(umi_project_workspace_workspace_json_render(&s,j,sizeof(j))==UMI_STATUS_OK);
    CHECK(strstr(j,"\"projects\":3")!=NULL);
    return 0;
}
