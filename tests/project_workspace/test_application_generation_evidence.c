#include "umicom/project/workspace/application_generation_evidence.h"
#define CHECK(x) do{if(!(x))return __LINE__;}while(0)
int main(void) {
    uint64_t a=umi_project_workspace_application_generation_evidence_fingerprint("plan");
    CHECK(a!=0U);
    CHECK(a==umi_project_workspace_application_generation_evidence_fingerprint("plan"));
    CHECK(a!=umi_project_workspace_application_generation_evidence_fingerprint("other"));
    return 0;
}
