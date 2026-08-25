#include "umicom/project/workspace/profile_selection.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceProfileCandidate c[2]= {
        0
    };
    size_t i=0U;
    c[0].capabilities=3U;
    c[0].priority=1U;
    c[0].enabled=true;
    c[1].capabilities=3U;
    c[1].priority=5U;
    c[1].enabled=true;
    CHECK(umi_project_workspace_profile_selection_best(c,2U,1U,&i)==UMI_STATUS_OK);
    CHECK(i==1U);
    return 0;
}
