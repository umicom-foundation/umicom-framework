#include "umicom/project/workspace/project_template_filter.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceTemplateCandidate c= {
        "c23-gtk","c","gtk4",3U
    };
    CHECK(umi_project_workspace_project_template_filter_score(&c,"c","gtk4",1U)>0);
    CHECK(umi_project_workspace_project_template_filter_score(&c,"cpp","gtk4",1U)<0);
    return 0;
}
