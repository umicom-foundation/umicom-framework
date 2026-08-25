#include "umicom/project/workspace/project_template_filter.h"
#include <string.h>
int umi_project_workspace_project_template_filter_score(const UmiProjectWorkspaceTemplateCandidate*item,const char*language,const char*frontend,uint64_t required_features) {
    int score=0;
    if(item==NULL||item->id==NULL)return -1;
    if((item->features&required_features)!=required_features)return -1;
    if(language!=NULL&&language[0]!='\0') {
        if(item->language==NULL||strcmp(item->language,language)!=0)return -1;
        score+=20;
    }
    if(frontend!=NULL&&frontend[0]!='\0') {
        if(item->frontend==NULL||strcmp(item->frontend,frontend)!=0)return -1;
        score+=10;
    }
    return score+(int)(item->features!=0U);
}
