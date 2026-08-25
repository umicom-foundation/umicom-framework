#include "umicom/project/workspace/application_generation_validation.h"
#include <string.h>
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    UmiProjectWorkspaceGenerationInput i= {
        "app","thin-desktop","C:/code",1U,1U
    };
    char m[64];
    CHECK(umi_project_workspace_application_generation_validation_evaluate(&i,m,sizeof(m))==UMI_STATUS_OK);
    CHECK(strcmp(m,"ready")==0);
    return 0;
}
